/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/*---------------------------------------------------------------------------*/
/**
 * \file
 *         Battery and Temperature IPv6 Demo for Zolertia Z1
 * \author
 *         Niclas Finne    <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Joel Hoglund    <joel@sics.se>
 *         Enric M. Calvo  <ecalvo@zolertia.com>
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "httpd-simple.h"
#include "webserver-nogui.h"
#include "dev/tmp102.h"
#include "dev/adxl345.h"
#include "dev/leds.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(web_sense_process, "Z1 Sense Web Demo");
AUTOSTART_PROCESSES(&web_sense_process);
/*---------------------------------------------------------------------------*/
#define HISTORY 16

static int temperature[HISTORY];
static int xaxis[HISTORY];
static int sensors_pos;
/*---------------------------------------------------------------------------*/
static int
get_xaxis(void)
{
  return adxl345.value(X_AXIS);
}
/*---------------------------------------------------------------------------*/
static int
get_mytemp(void)
{
  return tmp102.value(TMP102_READ);
}
/*---------------------------------------------------------------------------*/
static const char *TOP = "<html><head><title>Zolertia Web Sense</title></head><body>\n";
static const char *BOTTOM = "</body></html>\n";
/*---------------------------------------------------------------------------*/
/* Only one single request at time */
static char buf[256];
static int blen;
/*---------------------------------------------------------------------------*/
#define ADD(...) do { \
    blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__); \
} while(0)
/*---------------------------------------------------------------------------*/
static void
generate_chart(const char *title, const char *unit, int min, int max,
               int *values)
{
  int i;
  blen = 0;
  ADD("<h1>%s</h1>\n"
      "<img src=\"http://chart.apis.google.com/chart?"
      "cht=lc&chs=400x300&chxt=x,x,y,y&chxp=1,50|3,50&"
      "chxr=2,%d,%d|0,0,30&chds=%d,%d&chxl=1:|Time|3:|%s&chd=t:",
      title, min, max, min, max, unit);
  for(i = 0; i < HISTORY; i++) {
    ADD("%s%d", i > 0 ? "," : "", values[(sensors_pos + i) % HISTORY]);
  }
  ADD("\">");
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(send_values(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  SEND_STRING(&s->sout, TOP);

  /* Default page: show latest sensor values as text (does not
   * require Internet connection to Google for charts)
   */

  blen = 0;
  uint16_t x_axis = get_xaxis();
  uint16_t mytemp = get_mytemp();

  ADD("<h1>Current readings</h1>\n"
      "X axis: %d<br>"
      "Temperature: %ld.%02d &deg; C",
      (int8_t)x_axis,
      (long)(mytemp / 100), (unsigned)(mytemp % 100));
  SEND_STRING(&s->sout, buf);

  generate_chart("X axis", "None", -300, 300, xaxis);
  SEND_STRING(&s->sout, buf);

  generate_chart("Temperature", "Celsius", 0, 50, temperature);
  SEND_STRING(&s->sout, buf);

  SEND_STRING(&s->sout, BOTTOM);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
  return send_values;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(web_sense_process, ev, data)
{
  static struct etimer timer;
  PROCESS_BEGIN();

  sensors_pos = 0;
  process_start(&webserver_nogui_process, NULL);

  /* Activate sensors */
  SENSORS_ACTIVATE(adxl345);
  SENSORS_ACTIVATE(tmp102);

  etimer_set(&timer, CLOCK_SECOND * 2);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    /* Update the chars content */
    xaxis[sensors_pos] = get_xaxis();
    temperature[sensors_pos] = get_mytemp() / 100;

    /* Wrap-around and restart the counter */
    sensors_pos = (sensors_pos + 1) % HISTORY;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
