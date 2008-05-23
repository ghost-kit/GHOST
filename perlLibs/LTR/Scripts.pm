#!/usr/bin/perl

package LTR::Scripts;

BEGIN {eval}
use strict;
use POSIX;
use vars qw(@ISA @EXPORT @EXPORT_OK %EXPORT_TAGS $VERSION);

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);

@EXPORT = qw();

@EXPORT_OK =qw(
    writeutiobluesky
    writeutiobluevista
    writeutiolinux
    writelfmbluesky
    writelfmbluevista
    writelfmlinux
    writelfmparabluevista
    writelfmparalinux
    writelfmparajet
    writecmitbluesky
    writecmitbluevista
    writecmitlinux
    writecmitbu
    writelrbluesky
    writelrbluevista
    writelrlinux
);

%EXPORT_TAGS = (all => [@EXPORT_OK]);

sub writeutiobluesky {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#\@job_name         = $basename-%4.4d\n",$count;
   print OUT "#\@wall_clock_limit = $wallclock\n";
   printf OUT "#\@output           = $basename-%4.4d.out\n",$count;
   printf OUT "#\@error            = $basename-%4.4d.err\n",$count;
   print OUT "#\@job_type         = serial\n";
   print OUT "#\@node_usage       = $share\n";
   print OUT "#\@class            = $class\n";
   print OUT "#\@account_no       = $acctno\n";
   print OUT "#\@queue\n";
   print OUT "#\@ja_report        = no\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
   } else {
    print OUT "ln -sf INPUT1-start INPUT1\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "lfm-ut >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " llsubmit $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writeutiobluevista {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#BSUB -J $basename-%4.4d\n",$count;
   printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
   printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
   print OUT "#BSUB -N\n";
   print OUT "#BSUB -W $wallclock\n";
   print OUT "#BSUB -a openmp\n";
   print OUT "#BSUB -n $npes\n";
   print OUT "#BSUB -x\n";
   print OUT "#BSUB -q $class\n";
   print OUT "#BSUB -P $acctno\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
   } else {
    print OUT "ln -sf INPUT1-start INPUT1\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "lfm-ut >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writeutiolinux {
  my ($count,$basename,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
   } else {
    print OUT "ln -sf INPUT1-start INPUT1\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "./lfm-ut >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Starting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ./$basename-%4.4d.csh \&\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmbluesky {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#\@job_name         = $basename-%4.4d\n",$count;
   print OUT "#\@wall_clock_limit = $wallclock\n";
   printf OUT "#\@output           = $basename-%4.4d.out\n",$count;
   printf OUT "#\@error            = $basename-%4.4d.err\n",$count;
   print OUT "#\@job_type         = serial\n";
   print OUT "#\@node_usage       = $share\n";
   print OUT "#\@class            = $class\n";
   print OUT "#\@account_no       = $acctno\n";
   print OUT "#\@queue\n";
   print OUT "#\@ja_report        = no\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "lfm-hdf >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv DUMPHDF $basename-%4.4d.hdf\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ln -sf $basename-%4.4d.hdf TAKEHDF\n",$count;
   printf OUT " llsubmit $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmbluevista {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#BSUB -J $basename-%4.4d\n",$count;
   printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
   printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
   print OUT "#BSUB -N\n";
   print OUT "#BSUB -W $wallclock\n";
   print OUT "#BSUB -a openmp\n";
   print OUT "#BSUB -n $npes\n";
   print OUT "#BSUB -x\n";
   print OUT "#BSUB -q $class\n";
   print OUT "#BSUB -P $acctno\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "lfm-hdf >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv DUMPHDF $basename-%4.4d.hdf\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ln -sf $basename-%4.4d.hdf TAKEHDF\n",$count;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmlinux {
  my ($count,$basename,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "./lfm-hdf >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv DUMPHDF $basename-%4.4d.hdf\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Starting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ln -sf $basename-%4.4d.hdf TAKEHDF\n",$count;
   printf OUT " ./$basename-%4.4d.csh \&\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmparabluesky {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next,$nodes); 
   
# for jobs with less than 32 PEs uses 8 processor nodes
  
   if ($npes < 32)  {
     $nodes = ceil($npes/8);
   }
# for jobs with more than 32 PEs use 32 processor nodes
   if ($npes >= 32) {
     $nodes = ceil($npes/32);
   }

   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#\@job_name         = $basename-%4.4d\n",$count;
   print OUT "#\@wall_clock_limit = $wallclock\n";
   printf OUT "#\@output           = $basename-%4.4d.out\n",$count;
   printf OUT "#\@error            = $basename-%4.4d.err\n",$count;
   print OUT "#\@job_type         = parallel\n";
   print OUT "#\@network.MPI      = csss,not_shared,us\n";
   print OUT "#\@node             = $nodes\n";
   print OUT "#\@total_tasks      = $npes\n";
   print OUT "#\@node_usage       = $share\n";
   print OUT "#\@class            = $class\n";
   print OUT "#\@account_no       = $acctno\n";
   print OUT "#\@queue\n";
   print OUT "#\@ja_report        = no\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "LFM >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv DUMPHDF $basename-%4.4d.hdf\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ln -sf $basename-%4.4d.hdf TAKEHDF\n",$count;
   printf OUT " llsubmit $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmbluevista {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh  -f\n";
   printf OUT "#BSUB -J $basename-%4.4d\n",$count;
   printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
   printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
   print OUT "#BSUB -N\n";
   print OUT "#BSUB -W $wallclock\n";
   print OUT "#BSUB -a openmp\n";
   print OUT "#BSUB -n $npes\n";
   print OUT "#BSUB -x\n";
   print OUT "#BSUB -q $class\n";
   print OUT "#BSUB -P $acctno\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "lfm-hdf >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv DUMPHDF $basename-%4.4d.hdf\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ln -sf $basename-%4.4d.hdf TAKEHDF\n",$count;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmlinux {
  my ($count,$basename,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "./lfm-hdf >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv DUMPHDF $basename-%4.4d.hdf\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Starting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ln -sf $basename-%4.4d.hdf TAKEHDF\n",$count;
   printf OUT " ./$basename-%4.4d.csh \&\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmparabluevista {
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh  -f\n";
   printf OUT "#BSUB -J $basename-%4.4d\n",$count;
   printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
   printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
   print OUT "#BSUB -N\n";
   print OUT "#BSUB -W $wallclock\n";
   print OUT "#BSUB -a poe\n";
   print OUT "#BSUB -n $npes\n";
   print OUT "#BSUB -q $class\n";
   print OUT "#BSUB -P $acctno\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv XLFRTEOPS \"namelist=new\"\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d.xml INPUT1.xml\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "mpirun.lsf ./LFM >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   print OUT "foreach i (*.dmp)\n";
   print OUT " mv \$i \$i:r.hdf\n";
   print OUT "end\n";
#   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmparalinux {
  my ($count,$basename,$npes,$wrkdir) = @_;
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   printf OUT "mpirun -np $npes LFM >& lfm-$basename-%4.4d.msg \n",$count;
   print OUT "\n";
   print OUT "foreach i (*.dmp)\n";
   print OUT " mv \$i \$i:r.hdf\n";  
   print OUT "end\n";  
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Starting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ./$basename-%4.4d.csh \&\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelfmparajet {  
    my ($count,$basename,$wallclock,$acctno,$npes,$wrkdir) = @_;  
    my ($filename,$next);  
     
    $next = $count + 1;  
    $filename = sprintf "$basename-%4.4d.csh",$count;  
    open(OUT,">$filename") || die "Cannot open $filename";  
    print OUT "#!/bin/csh  -f\n";  
    printf OUT "#\$ -N $basename-%4.4d\n",$count;  
    printf OUT "#\$ -o $basename-%4.4d.output\n",$count;  
    printf OUT "#\$ -e $basename-%4.4d.error\n",$count;  
    print OUT "#\$ -A $acctno\n";  
    print OUT "#\$ -l h_rt=$wallclock\n";  
    print OUT "#\$ -pe wcomp $npes\n";  
    print OUT "\n";  
    print OUT "cd $wrkdir\n";  
    print OUT "\n";  
    print OUT "#Record the starting time of this portion of the run\n";  
    print OUT "\n";  
    printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",  
    $count,$count;  
    printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;  
    print OUT "\n";  
    print OUT "#setup the input files\n";  
    printf OUT "ln -sf INPUT1-%4.4d INPUT1\n",$count;  
    print OUT "\n";  
    print OUT "#run the code\n";  
    printf OUT "mpirun -np $npes LFM >& lfm-$basename-%4.4d.msg \n",$count;  
    print OUT "\n";  
    print OUT "foreach i (*.dmp)\n";  
    print OUT " mv \$i \$i:r.hdf\n";  
    print OUT "end\n";  
    print OUT "\n";  
    print OUT "# record the ending time of this portion of the run\n";  
    printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",  
    $count,$count;  
    printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;  
    print OUT "\n";  
    print OUT "# look and see if there is more to do\n";  
    printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;  
    printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;  
    printf OUT "  qsub < $basename-%4.4d.csh\n",$next;  
    print OUT "endif\n";  
    print OUT "\n";  
    close(OUT);  
    chmod 0755,"$filename";  
}

sub writecmitbu {
# This creates scripts for submitting bsub jobs to the BU machines
# The count=0 version creates the TING startup file and runs the LFM to just 
# before the coupling interval
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$wallclock,$class,$npes,$wrkdir,$ic) = @_;
  my ($val);
  my ($filename,$next);

  $next = $count + 1;
  $filename = sprintf "$basename-%4.4d.csh",$count;
  open(OUT,">$filename") || die "Cannot open $filename";

  
  print OUT "#!/bin/csh -f\n";
  printf OUT "#BSUB -J $basename-%4.4d\n",$count;
  print OUT "#BSUB -q $class\n";
  print OUT "#BSUB -m jacks playdoh\n";
  printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
  printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
  print OUT "#BSUB -W $wallclock\n";

   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-restart INPUT-CMIT\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   printf OUT "ln -sf inputdata-%4.4d inputdata\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       printf OUT "ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "ting-adhoc >& ting-$basename-%4.4d.msg \n",$count;
       printf OUT "lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       printf OUT "ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "ting-adhoc >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv condte.dat condte-$basename-%4.4d.dat\n",$count;
   printf OUT "mv condvy.dat condvy-$basename-%4.4d.dat\n",$count;
   printf OUT "mv jouleh.dat jouleh-$basename-%4.4d.dat\n",$count;
   printf OUT "mv inputs.dat inputs-$basename-%4.4d.dat\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
               $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writecmitbluesky {
# This creates scripts for submitting to loadleveler on bluesky
# The count=0 version creates the TING startup file and runs the LFM to 
# just before the coupling interval.
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir,$ic) = @_;
  my ($val);
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   printf OUT "#\@job_name         = $basename-%4.4d\n",$count;
   print OUT "#\@wall_clock_limit = $wallclock\n";
   printf OUT "#\@output           = $basename-%4.4d.out\n",$count;
   printf OUT "#\@error            = $basename-%4.4d.err\n",$count;
   print OUT "#\@job_type         = serial\n";
   print OUT "#\@node_usage       = $share\n";
   print OUT "#\@class            = $class\n";
   print OUT "#\@account_no       = $acctno\n";
   print OUT "#\@queue\n";
   print OUT "#\@ja_report        = no\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-restart INPUT-CMIT\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   printf OUT "ln -sf inputdata-%4.4d inputdata\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       printf OUT "ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "ting-adhoc >& ting-$basename-%4.4d.msg \n",$count;
       printf OUT "lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       printf OUT "ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "ting-adhoc >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv condte.dat condte-$basename-%4.4d.dat\n",$count;
   printf OUT "mv condvy.dat condvy-$basename-%4.4d.dat\n",$count;
   printf OUT "mv jouleh.dat jouleh-$basename-%4.4d.dat\n",$count;
   printf OUT "mv inputs.dat inputs-$basename-%4.4d.dat\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
               $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " llsubmit $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writecmitbluevista {
# This creates scripts for submitting to loadleveler on bluesky
# The count=0 version creates the TING startup file and runs the LFM to 
# just before the coupling interval.
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir,$ic) = @_;
  my ($val);
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   printf OUT "#BSUB -J $basename-%4.4d\n",$count;
   printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
   printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
   print OUT "#BSUB -N\n";
   print OUT "#BSUB -W $wallclock\n";
   print OUT "#BSUB -a openmp\n";
   print OUT "#BSUB -n $npes\n";
   print OUT "#BSUB -x\n";
   print OUT "#BSUB -q $class\n";
   print OUT "#BSUB -P $acctno\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
              $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-restart INPUT-CMIT\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   printf OUT "ln -sf inputdata-%4.4d inputdata\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       printf OUT "ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "ting-adhoc >& ting-$basename-%4.4d.msg \n",$count;
       printf OUT "lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       printf OUT "ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "ting-adhoc >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv condte.dat condte-$basename-%4.4d.dat\n",$count;
   printf OUT "mv condvy.dat condvy-$basename-%4.4d.dat\n",$count;
   printf OUT "mv jouleh.dat jouleh-$basename-%4.4d.dat\n",$count;
   printf OUT "mv inputs.dat inputs-$basename-%4.4d.dat\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
               $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}
sub writecmitlinux {
# This creates c shell scripts from running on linux or other interactive mach
# The count=0 version creates the TING startup file and runs the LFM to 
# just before the coupling interval.
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$npes,$wrkdir,$ic) = @_;
  my ($val);
  my ($filename,$next); 
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   
   print OUT "#!/bin/csh -f\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-start INPUT-CMIT\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-CMIT-restart INPUT-CMIT\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   printf OUT "ln -sf inputdata-%4.4d inputdata\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       printf OUT "./ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "./lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "./ting-adhoc >& ting-$basename-%4.4d.msg \n",$count;
       printf OUT "./lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       printf OUT "./ting-ic >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "./lfm-cmit-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       printf OUT "./ting-adhoc >& ting-$basename-%4.4d.msg &\n",$count;
       printf OUT "./lfm-cmit-adhoc >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "mv condte.dat condte-$basename-%4.4d.dat\n",$count;
   printf OUT "mv condvy.dat condvy-$basename-%4.4d.dat\n",$count;
   printf OUT "mv jouleh.dat jouleh-$basename-%4.4d.dat\n",$count;
   printf OUT "mv inputs.dat inputs-$basename-%4.4d.dat\n",$count;
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
              $count, $count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Starting $basename-%4.4d.csh\"\n",$next;
   printf OUT " ./$basename-%4.4d.csh \&\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelrbluesky {
# This creates scripts for submitting to loadleveler on bluesky
# The count=0 version creates the TING startup file and runs the LFM to 
# just before the coupling interval.
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir,$ic) = @_;
  my ($val,$filename,$next,$pid);
   
   $next = $count+1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#\@job_name         = $basename-%4.4d\n",$count;
   print OUT "#\@wall_clock_limit = $wallclock\n";
   printf OUT "#\@output           = $basename-%4.4d.out\n",$count;
   printf OUT "#\@error            = $basename-%4.4d.err\n",$count;
   print OUT "#\@job_type         = serial\n";
   print OUT "#\@node_usage       = $share\n";
   print OUT "#\@class            = $class\n";
   print OUT "#\@queue\n";
   print OUT "#\@ja_report        = no\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-RCM-start INPUT-RCM\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-RCM-start INPUT-RCM\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-RCM-restart INPUT-RCM\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "rcm_lfm-ic >& rcm-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-rcm-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm-ic >& lfm-$basename-%4.4d.msg &\n",$count;
       printf OUT "rcm_lfm-ic >& rcm-$basename-%4.4d.msg \n",$count;
     } else {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm >& lfm-$basename-%4.4d.msg &\n",$count;
       printf OUT "rcm_lfm >& rcm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "cp lfminfor.bin lfminfor.bin-$basename-%4.4d\n",$count;
   printf OUT "cp lfminfor.dat lfminfor.dat-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# clean up in case of rouge processes\n";
   print OUT "# For whatever reason if the processes are already dead then\n";
   print OUT "# a kill command without arguments stops the entire script\n";
   print OUT "sleep 60\n";
   print OUT "setenv pid `ps -u \$USER | grep rcm_lfm | awk '{print \$2}'`\n"; 
   print OUT "if (\$pid > 1) then\n";
   print OUT "  kill -9 \$pid\n";
   print OUT "endif\n";
   print OUT "setenv pid `ps -u \$USER | grep lfm-rcm | awk '{print \$2}'`\n"; 
   print OUT "if (\$pid > 1) then\n";
   print OUT "  kill -9 \$pid\n";
   print OUT "endif\n";
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " llsubmit $basename-%4.4d.csh\n",$next;
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelrbluevista {
# This creates scripts for submitting to loadleveler on bluesky
# The count=0 version creates the TING startup file and runs the LFM to 
# just before the coupling interval.
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$wallclock,$share,$class,$acctno,$npes,$wrkdir,$ic) = @_;
  my ($val,$filename,$next,$pid);
   
   $next = $count+1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   printf OUT "#BSUB -J $basename-%4.4d\n",$count;
   printf OUT "#BSUB -o $basename-%4.4d.output\n",$count;
   printf OUT "#BSUB -e $basename-%4.4d.error\n",$count;
   print OUT "#BSUB -N\n";
   print OUT "#BSUB -W $wallclock\n";
   print OUT "#BSUB -a openmp\n";
   print OUT "#BSUB -n $npes\n";
   print OUT "#BSUB -x\n";
   print OUT "#BSUB -q $class\n";
   print OUT "#BSUB -P $acctno\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "setenv XLFRTEOPTS \"NAMELIST=OLD\"\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-RCM-start INPUT-RCM\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-RCM-start INPUT-RCM\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-RCM-restart INPUT-RCM\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "rcm_lfm-ic >& rcm-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-rcm-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm-ic >& lfm-$basename-%4.4d.msg &\n",$count;
       printf OUT "rcm_lfm-ic >& rcm-$basename-%4.4d.msg \n",$count;
     } else {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm >& lfm-$basename-%4.4d.msg &\n",$count;
       printf OUT "rcm_lfm >& rcm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "cp lfminfor.bin lfminfor.bin-$basename-%4.4d\n",$count;
   printf OUT "cp lfminfor.dat lfminfor.dat-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# clean up in case of rouge processes\n";
   print OUT "# For whatever reason if the processes are already dead then\n";
   print OUT "# a kill command without arguments stops the entire script\n";
   print OUT "sleep 60\n";
   print OUT "setenv pid `ps -u \$USER | grep rcm_lfm | awk '{print \$2}'`\n"; 
   print OUT "if (\$pid > 1) then\n";
   print OUT "  kill -9 \$pid\n";
   print OUT "endif\n";
   print OUT "setenv pid `ps -u \$USER | grep lfm-rcm | awk '{print \$2}'`\n"; 
   print OUT "if (\$pid > 1) then\n";
   print OUT "  kill -9 \$pid\n";
   print OUT "endif\n";
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Submitting $basename-%4.4d.csh\"\n",$next;
   printf OUT " bsub < $basename-%4.4d.csh\n",$next;
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

sub writelrlinux {
# This creates c shell scripts from running on linux or other interactive mach
# The count=0 version creates the TING startup file and runs the LFM to 
# just before the coupling interval.
# All subsequent files run out sequences of delta in length
  my ($count,$basename,$npes,$wrkdir,$ic) = @_;
  my ($val,$filename,$next,$pid);
   
   $next = $count + 1;
   $filename = sprintf "$basename-%4.4d.csh",$count;
   open(OUT,">$filename") || die "Cannot open $filename";
   print OUT "#!/bin/csh -f\n";
   print OUT "\n";
   print OUT "\n";
   print OUT "setenv OMP_NUM_THREADS $npes\n";
   print OUT "\n";
   print OUT "cd $wrkdir\n";
   print OUT "\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo quit | pvm \n";
   }
   print OUT "#Record the starting time of this portion of the run\n";
   print OUT "\n";
   printf OUT "echo \"$basename-%4.4d started at\" > $basename-%4.4d.status\n",
             $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "#setup the input files\n";
   if ($count == 0) {
    print OUT "ln -sf INPUT1-start INPUT1\n";
    print OUT "ln -sf INPUT-RCM-start INPUT-RCM\n";
   }
   if ($count == 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-RCM-start INPUT-RCM\n";
   }
   if ($count > 1) {
    print OUT "ln -sf INPUT1-restart INPUT1\n";
    print OUT "ln -sf INPUT-RCM-restart INPUT-RCM\n";
   }
   printf OUT "ln -sf INPUT-UTIO-%4.4d INPUT-UTIO\n",$count;
   print OUT "\n";
   print OUT "#run the code\n";
   
   if ($count == 0) {
     if ($val = $ic =~ /true/) {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "rcm_lfm-ic >& rcm-$basename-%4.4d.msg &\n",$count;
       printf OUT "lfm-rcm-ic >& lfm-$basename-%4.4d.msg \n",$count;
     } else {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm >& lfm-$basename-%4.4d.msg \n",$count;
     }
   } else {
     if ($val = $ic =~ /true/) {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm-ic >& lfm-$basename-%4.4d.msg &\n",$count;
       printf OUT "rcm_lfm-ic >& rcm-$basename-%4.4d.msg \n",$count;
     } else {
       print OUT "touch rcm_pause.dat\n";
       printf OUT "lfm-rcm >& lfm-$basename-%4.4d.msg &\n",$count;
       printf OUT "rcm_lfm >& rcm-$basename-%4.4d.msg \n",$count;
     }
   } 
   print OUT "wait\n";
   if ($val = $ic =~ /true/) {
     print OUT "echo halt | pvm \n";
   }
   print OUT "\n";
   printf OUT "mv LOGFILE LOGFILE-$basename-%4.4d\n",$count;
   printf OUT "cp lfminfor.bin lfminfor.bin-$basename-%4.4d\n",$count;
   printf OUT "cp lfminfor.dat lfminfor.dat-$basename-%4.4d\n",$count;
   print OUT "\n";
   print OUT "# clean up in case of rouge processes\n";
   print OUT "sleep 60\n";
   print OUT "setenv pid `ps -u \$USER | grep rcm_lfm | awk '{print \$2}'`\n"; 
   print OUT "if (\$pid > 1) then\n";
   print OUT "  kill -9 \$pid\n";
   print OUT "endif\n";
   print OUT "setenv pid `ps -u \$USER | grep lfm-rcm | awk '{print \$2}'`\n"; 
   print OUT "if (\$pid > 1) then\n";
   print OUT "  kill -9 \$pid\n";
   print OUT "endif\n";
   print OUT "\n";
   print OUT "# record the ending time of this portion of the run\n";
   printf OUT "echo \"$basename-%4.4d ended at\" >> $basename-%4.4d.status\n",
          $count,$count;
   printf OUT "echo `date` >> $basename-%4.4d.status\n",$count;
   print OUT "\n";
   print OUT "# look and see if there is more to do\n";
   printf OUT "if (-x $basename-%4.4d.csh) then\n",$next;
   printf OUT " echo \"Starting $basename-%4.4d.csh\"\n",$next;
   printf OUT " $basename-%4.4d.csh \&\n",$next;
   print OUT "endif\n";
   print OUT "\n";
   close(OUT);
   chmod 0755,"$filename";
}

1;
__END__
