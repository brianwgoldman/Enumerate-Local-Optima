#!/usr/bin/Rscript
library(ggplot2)
library(scales)

# Some basic tools for making pretty graphs
log_x <- scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x),
                       labels = trans_format("log10", math_format(10^.x)))
log_y <- scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x),
                       labels = trans_format("log10", math_format(10^.x)))

clean <- theme_bw() + theme(
    plot.background = element_blank()
   ,panel.grid.major = element_blank()
   ,panel.grid.minor = element_blank()
   ,panel.border = element_blank()
  ) + theme(axis.line = element_line(color = 'black'))

# Colors used for the 3 different methods
three_color <- c("#1b9e77", "#d95f02", "#7570b3")

# Read in the data and order the problems
data <- read.csv("data.csv", header=T)
data$Problem <- ordered(data$Problem, levels=c("Concatenated Traps", "Adjacent NKq", "Random NKq", "Ising Spin Glass", "MAX-kSAT"))
data$Radius <- factor(data$Radius)

# Determine how many of each method was successful
successes <- aggregate(Seed~Problem+Length+k+Method+Radius, data, FUN=length)
colnames(successes)[colnames(successes)=="Seed"] <- "Successes"

# Find only methods problem+method+radius combinations that were fully successful
fully_successful <- subset(successes, Successes == 30)
# Data from only fully successful sizes
successful_at_size <- merge(fully_successful, data)
print("Largest size each method + radius was able to solve 100% of runs")
print(aggregate(Length~k+Radius+Problem+Method, fully_successful, FUN=max))


means <- aggregate(Seconds~Problem+Length+k+Method+Radius, successful_at_size, FUN=mean)
best_means <- aggregate(Seconds~Problem+Length+k+Radius, means, FUN=min)
cat("\nProblems where Hyper-Reorder wasn't the best method\n")
print(subset(merge(best_means, means), Method != "Hyper-Reorder"))

# Output the model coefficients in LaTeX table format
cat("\nTable 1 in LaTeX format\n")
options(digits=4)
for (p in levels(data$Problem)) {
  cat(p, "& ")
  for (m in levels(data$Method)) {    
    model <- lm(log2(Seconds)~Length, subset(successful_at_size, Method==m & Radius==1 & Problem==p))
    cat(coef(model)[2], "& ")
  }
  model <- lm(log2(Count)~Length, subset(successful_at_size, Method=="Hyper-Reorder" & Radius==1 & Problem==p))
  cat(coef(model)[2], "\\\\ \\hline\n")
}
cat('\n')
# Estimate how long it takes Gray-Box to do big problems
gba <- lm(log2(Seconds)~Length, subset(successful_at_size, Method=="Gray-Box" & Radius==1 & Problem=="Adjacent NKq"))
cat("Gray-Box estimated Adjacent NKq N=77: ", 2^(coef(gba)[1]) * 2^(coef(gba)[2] * 77), " seconds\n")
gbr <- lm(log2(Seconds)~Length, subset(successful_at_size, Method=="Gray-Box" & Radius==1 & Problem=="Random NKq"))
cat("Gray-Box estimated Random NKq N=69: ", 2^(coef(gbr)[1]) * 2^(coef(gbr)[2] * 69), " seconds\n")

# Data for Adjacent NKq 200
large <- read.csv("adjacent_nkq_200.csv", header=TRUE)
cat("\nHyper-Reorder on Adjacent NKq N=200 Radius=4: ", mean(large$Seconds), " seconds\n")
print("Seconds each run required:")
print(sort(large$Seconds))
print("Number of local optima:")
print(sort(large$Count))

############## Make Figures #############
# Figure 2
all_three_successful <- subset(aggregate(Seed~Problem+Length+k+Radius, data, FUN=length), Seed == 30*3)
all_three_largest_sizes <- aggregate(Length~k+Radius+Problem, all_three_successful, FUN=max)
all_three_largest <- merge(all_three_largest_sizes, data)
all_three_largest$NLength <- paste("N=", all_three_largest$Length, sep="")

plt <- ggplot(subset(all_three_largest, Radius==1), aes(Method, Seconds, color=Method)) +
       geom_boxplot() + facet_wrap(~Problem+NLength, ncol=3, scale="free_x") +
       log_y + clean +
       scale_color_manual(values=three_color) + theme(legend.position = "none")
ggsave("boxplot-method.eps", plt, width=8, height=5)

# Figure 3
plt <- ggplot(subset(successful_at_size, Radius==1), aes(Length, Seconds, color=Method, shape=Method)) + 
       stat_smooth(method="lm", show_guide=FALSE) + 
       stat_summary(fun.y=mean, geom="point") +
       facet_wrap(~Problem, ncol=3, scale="free_x") + scale_color_manual(values=three_color) +
       clean + log_y +
       theme(legend.position = c(5/6, .25), legend.justification = c(.5, 0.5))
ggsave("length-method.eps", plt, width=8, height=5)

# Figure 4
modeled <- NULL
for (p in levels(successful_at_size$Problem)) {
  for (m in levels(successful_at_size$Method)) {    
    for (r in levels(successful_at_size$Radius)) {
      model <- lm(log2(Seconds)~Length, subset(successful_at_size, Method==m & Radius==r & Problem==p))
      values = as.numeric(coef(model))
      row <- data.frame(Intercept=values[1], Slope=values[2],
                        Lower=confint(model)[2,1], Upper=confint(model)[2,2], 
                        Rsquared=summary(model)$r.squared,
                        Problem=p, Method=m, Radius=r)
      modeled <- rbind(modeled, row)
    }
  }
}

plt <- ggplot(modeled, aes(Radius, y=Slope, ymin=Lower, ymax=Upper, color=Method, shape=Method)) +
       geom_errorbar(show_guide=FALSE) + geom_point() +
       facet_wrap(~Problem, ncol=3, scale="free_x") + 
       clean + scale_color_manual(values=three_color) +
       theme(legend.position = c(5/6, .25), legend.justification = c(.5, 0.5))
ggsave("slope-method.eps", plt, width=8, height=5)

# Figure 5
five_color <- c("#000000", "#E69F00", "#56B4E9", "#D55E00", "#CC79A7") #"#F0E442", "#0072B2", (not bad) "#009E73",
successful_at_size$RRadius <- factor(paste("R", successful_at_size$Radius, sep="="))
plt <- ggplot(subset(successful_at_size, Method=="Hyper-Reorder"), aes(Length, Count, color=Problem, shape=Problem)) + 
       stat_smooth(method="lm", show_guide=FALSE) + 
       stat_summary(fun.y=mean, geom="point") +
       facet_wrap(~RRadius, ncol=3, scale="free_x") +
       clean + log_y + scale_color_manual(values=five_color) +
       theme(legend.position = c(5/6, .25), legend.justification = c(.5, 0.5))
ggsave("length-radius.eps", plt, width=8, height=5)

