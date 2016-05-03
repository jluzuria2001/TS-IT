### plot a grap with a interval confidence

add.error.bars <- function(X,upper,lower,width,col=par( )$fg,lwd=1){
  segments(X,lower,X,upper,col=col,lwd=lwd,lend=1);
  segments(X-width/2,lower,X+width/2,lower,col=col,lwd=lwd,lend=1);
  segments(X-width/2,upper,X+width/2,upper,col=col,lwd=lwd,lend=1);
}

X = 1:2; 
Y = c(20.39, 41.10); 
upper=c( 28, 222); 
lower=c(19.96, 41.10); 

plot.new();

#Set the limits for the plotting window axes.
par(usr=c(0,3, 0,50));

#Draw the horizontal axis.
axis(1, at = c(1, 2), labels=c("BR (1 hop)", "Mote (2 hops)"));
     
#Draw the vertical axis.
axis(2,las=1); 

#Draw a frame around the plot.
box(bty=letters[12], lwd=1); 
#Add the title.
title(main=quote("Ping Response Times"),cex.main=1,font.main=1, xlab="destination", ylab="RTT (ms)"); 
#Plot the average data points.
points(X,Y); 

#Draw the error bars.
add.error.bars(X,lower,upper,width=0.4,col=rgb(0,0,1));
