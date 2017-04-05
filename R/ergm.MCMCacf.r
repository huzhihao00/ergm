ergm.MCMCacf<-function(statsmatrix, lag.max=50)
{
  if(ncol(statsmatrix)==1){
   av <- mean(statsmatrix)
   xsim <- statsmatrix-av
   z <- xsim+av
   lag.max <- min(round(sqrt(nrow(xsim))),lag.max)
   corV <- as.matrix(acf(z, lag.max = lag.max,
    type = "correlation", plot = FALSE)$acf[2,,])
   dimnames(corV) <- list(colnames(statsmatrix),colnames(statsmatrix))
   return(corV)
  }else{
   av <- apply(statsmatrix, 2, mean)
   xsim <- sweep(statsmatrix, 2, av, "-")
  }
#
#  Determine the correlation function
#
#  require("ts", quietly = TRUE, keep.source = FALSE)
   z <- sweep(xsim, 2, av, "+")
   lag.max <- min(round(sqrt(nrow(xsim))),lag.max)
   corV <- acf(z, lag.max = lag.max,
    type = "correlation", plot = FALSE)$acf[1:2,,]
   if(is.array(corV)){
     dimnames(corV) <- list(c("cor","lag1"), colnames(statsmatrix), 
                                             colnames(statsmatrix))
     corV <- aperm(corV)
   }else{
     names(corV) <- c("1",colnames(statsmatrix))
   }
   corV
}
