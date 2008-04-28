#!/usr/bin/perl

package LTR::Inputs;

BEGIN {eval}
use strict;
use vars qw(@ISA @EXPORT @EXPORT_OK %EXPORT_TAGS $VERSION);

$VERSION='1.0';

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);

@EXPORT = qw();

@EXPORT_OK =qw(
    writeutio
    writeinputcmit
    writeinput1
    writeinput1l
    writeinput1lp
    writeinput1r
    writeinputdata
    writedinput
    writeinputrcm
    writeinputmf
    writeinput1xml
);

%EXPORT_TAGS = (all => [@EXPORT_OK]);
    
sub writeutio {
# This subroutine creates the INPUT-UTIO files with the following args
  my($count,$basename,$dumpint,
     $eventyr,$eventmon,$eventday,$eventhr,$eventmin,$eventsec,
     $startyr,$startmon,$startday,$starthr,$startmin,$startsec,
     $stopyr,$stopmon,$stopday,$stophr,$stopmin,$stopsec) = @_;
  my ($filename); 
   
  $filename = sprintf ">INPUT-UTIO-%4.4d",$count;
  open(OUT,"$filename") || die "Cannot open $filename";

  print OUT " &UTIO\n";
  print OUT "  ieventyr=$eventyr,\t\t! Year of Event\n";
  print OUT "  ieventmon=$eventmon,\t\t\t! Month of Event\n";
  print OUT "  ieventday=$eventday,\t\t\t! Day of Event\n";
  print OUT "  ieventhr=$eventhr,\t\t\t! Hr of Event\n";
  print OUT "  ieventmin=$eventmin,\t\t\t! Min of Event\n";
  print OUT "  ieventsec=$eventsec,\t\t\t! Sec of Event\n";
  print OUT "  istartyr=$startyr,\t\t! Starting Year of Interval\n";
  print OUT "  istartmon=$startmon,\t\t\t! Starting Month of Interval\n";
  print OUT "  istartday=$startday,\t\t\t! Starting Day of Interval\n";
  print OUT "  istarthr=$starthr,\t\t\t! Starting Hr of Interval\n";
  print OUT "  istartmin=$startmin,\t\t\t! Starting Min of Interval\n";
  print OUT "  istartsec=$startsec,\t\t\t! Starting Sec of Interval\n";
  print OUT "  istopyr=$stopyr,\t\t\t! Final Year of Interval\n";
  print OUT "  istopmon=$stopmon,\t\t\t! Final Month of Interval\n";
  print OUT "  istopday=$stopday,\t\t\t! Final Day of Interval\n";
  print OUT "  istophr=$stophr,\t\t\t! Final Hr of Interval\n";
  print OUT "  istopmin=$stopmin,\t\t\t! Final Min of Interval\n";
  print OUT "  istopsec=$stopsec,\t\t\t! Final Sec of Interval\n";
  print OUT "  rdumpint=$dumpint,\t\t! Seconds between dumps\n";
  print OUT "  rcorfac=-0.8333,\t\t! Correction factor\n";
  print OUT "  cabase=\'$basename\'\t\t\t! Base name of output files\n";
  print OUT " &END\n";
  close(OUT);
}

sub writeinputcmit {
#This subroutine creates the INPUT-CMIT-start and INPUT-CMIT-restart files
#with the following arguments
  my ($cmityr,$cmitmon,$cmitday,$cmithr,$cmitmin,$cmitsec,
      $tingstep) = @_;
  my (@restart,$restart,$val);
  @restart = (".true.",".false.");
  foreach $restart (@restart) {
  if ($val = $restart =~ /false/) {
    open(OUT,">INPUT-CMIT-start") || die "Cannot open INPUT-start";
  } else {
    open(OUT,">INPUT-CMIT-restart") || die "Cannot open INPUT-start";
  }
  print OUT " &TINGIN\n";
  print OUT "  iTingYr=$cmityr,\t\t\t! Year to start coupling with TING\n";
  print OUT "  iTingMon=$cmitmon,\t\t\t! Month to start coupling with TING\n";
  print OUT "  iTingDay=$cmitday,\t\t\t! Day to start coupling with TING\n";
  print OUT "  iTingHr=$cmithr,\t\t\t! Hour to start coupling with TING\n";
  print OUT "  iTingMin=$cmitmin,\t\t\t! Minute to start coupling with TING\n";
  print OUT "  iTingSec=$cmitsec,\t\t\t! Second to start coupling with TING\n";
  print OUT "  rDelta=$tingstep,\t\t\t! Seconds between LFM/TING exchanges\n";
  print OUT "  restartTing=$restart,\t\t! Restarting in TING interval\n";
  print OUT "  nwind=.false.,\t\t! Include Neutral Wind Feedback\n";
  print OUT "  caToDir='./',\t\t\t! Directory for file exchanges\n";
  print OUT " &END\n";
  close(OUT);
  }
}

sub writeinput1 {
#This subroutine creates the INPUT1-start and INPUT1-restart files
#with the following arguments
  my ($nrings,$naver,$bx,$by,$bz,$sm,$tiltit,$iday,$time_of_day,
      $ionmodel,$f107,$ped0,$it) = @_;
 
  my (@restart,$restart,$val);
  @restart = (".true.",".false.");
  foreach $restart (@restart) {
    if ($val = $restart =~ /false/) {
      open(OUT,">INPUT1-start") || die "Cannot open INPUT-start";
    } else {
      open(OUT,">INPUT1-restart") || die "Cannot open INPUT-start";
    }
    print OUT " &GO\n";
    print OUT "  RSTRT=$restart,\t\t\t!Restart from previous HDF File\n";
    print OUT "  NSTART=0,\t\t\t!Initial Step of this segment\n";
    print OUT "  NSTOP=100,\t\t\t!Final Step of this segment\n";
    print OUT "  NDUMP=25,\t\t\t!Frequency of dumps to HDF File\n";
    print OUT "  ORDER=8,\t\t\t!Numerical order of calculation\n";
    print OUT "  NRINGS=$nrings,\t\t\t!Number of averaging bins\n";
    print OUT "  NAVER=$naver,\t\t!Size of avg bins (must be nj in size)\n";
    print OUT "  GEOQMU=0.30,\t\t\t!Strength of B field at pole\n";
    print OUT " &END\n";
    print OUT " &CONST\n";
    print OUT "  GAMMA=1.66666667,\t\t!Thermo gas constant\n";
    print OUT "  CA=1.1e8,\t\t\t!Speed of light for Borris Correction\n";
    print OUT "  CRNT=0.3,\t\t\t!Cournt Number\n";
    print OUT "  PDMB=4.,\t\t\t!PDM Noliner Switch Constant\n";
    print OUT "  AXFIX=0.5,\t\t\t!Normalized B Field Diff Coeff around Axis\n";
    print OUT " &END\n";
    print OUT " &SWIND\n";
    print OUT "  rho_start=2.25e-25,\n";
    print OUT "  c_start=1.5e6,\n";
    print OUT "  rho_0 = 1.125e-23,\t\t!Initial value of solar wind density\n";
    print OUT "  vx_0 = -4.0e7,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  vy_0 = 0.0,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  vz_0 = 0.0,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  c_0 = 3.5e6,\t\t\t!Initial value of solar wind sound speed\n";
    print OUT "  bx_0 = 0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  by_0 = 0.0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  bz_0 = 0.0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  tzero=3000.0,\t\t\t!Secs of Initial solar wind conditions\n";
    print OUT "  by_coef=$by,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  bz_coef=$bz,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  bx_zero=${bx}e-5,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  GSM=.true.,\t\t\t!Perform calcs in GSM coords\n";
    print OUT "  SM=$sm,\t\t\t!Solar Wind file in SM coords\n";
    print OUT "  tilt_it=$tiltit,\t\t!Use fixed dipole tilt\n";
    print OUT "  iday=$iday,\t\t\t!Day and Month of fixed dipole tilt\n";
    print OUT "  time_of_day=$time_of_day,\t\t!UT of fixed dipole tilt\n";
    print OUT " &END\n";
    print OUT " &ION\n";
    print OUT "  ionmodel=$ionmodel,\t\t!Use empirical ionospheric model\n";
    print OUT "  S107=$f107,\t\t\t!10.7 Flux\n";
    print OUT "  hallmin=1.0,\t\t\t!Minimum value for Hall Conductivity\n";
    print OUT "  pedmin=2.0,\t\t\t!Minimum value for Pedersen Conductivity\n";
    print OUT "  SIGMA_RATIO=1.0,\t\t!Max Ratio of Hall to Ped Conductity\n";
    print OUT "  P_HARDY=2.24,\t\t\t!Empirical flux constant\n";
    print OUT "  C_HARDY=1.0,\t\t\t!Empirical flux constant\n";
    print OUT "  H_HARDY=1.333,\t\t!Empirical flux constant\n";
    print OUT "  ped0=$ped0,\t\t\t!Value of Ped if no ion model\n";
    print OUT "  iter = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  itmax = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  ERRMAX=2.E-3,\t\t\t!Error certerion in ellipitic solve\n";
    print OUT "  ion_skip=10,\t\t\t!Recalc Ionosphere every N steps\n";
    print OUT " &END\n";
    close(OUT);
  }
}

sub writeinput1l {
#This subroutine creates the INPUT1-start and INPUT1-restart files
#with the following arguments
  my ($nrings,$naver,$bx,$by,$bz,$sm,$tiltit,$iday,$time_of_day,
      $ionmodel,$f107,$ped0,$it,$restart,$start,$stop,$dump,$count) = @_;

  my($filename);
 
  $filename = sprintf ">INPUT1-%4.4d",$count;
  open(OUT,"$filename") || die "Cannot open $filename";
    print OUT " &GO\n";
    print OUT "  RSTRT=$restart,\t\t\t!Restart from previous HDF File\n";
    print OUT "  NSTART=$start,\t\t\t!Initial Step of this segment\n";
    print OUT "  NSTOP=$stop,\t\t\t!Final Step of this segment\n";
    print OUT "  NDUMP=$dump,\t\t\t!Frequency of dumps to HDF File\n";
    print OUT "  ORDER=8,\t\t\t!Numerical order of calculation\n";
    print OUT "  NRINGS=$nrings,\t\t\t!Number of averaging bins\n";
    print OUT "  NAVER=$naver,\t\t!Size of avg bins (must be nj in size)\n";
    print OUT "  GEOQMU=0.30,\t\t\t!Strength of B field at pole\n";
    print OUT " &END\n";
    print OUT " &CONST\n";
    print OUT "  GAMMA=1.66666667,\t\t!Thermo gas constant\n";
    print OUT "  CA=1.1e8,\t\t\t!Speed of light for Borris Correction\n";
    print OUT "  CRNT=0.3,\t\t\t!Cournt Number\n";
    print OUT "  PDMB=4.,\t\t\t!PDM Noliner Switch Constant\n";
    print OUT "  AXFIX=0.5,\t\t\t!Normalized B Field Diff Coeff around Axis\n";
    print OUT " &END\n";
    print OUT " &SWIND\n";
    print OUT "  rho_start=2.25e-25,\n";
    print OUT "  c_start=1.5e6,\n";
    print OUT "  rho_0 = 1.125e-23,\t\t!Initial value of solar wind density\n";
    print OUT "  vx_0 = -4.0e7,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  vy_0 = 0.0,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  vz_0 = 0.0,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  c_0 = 3.5e6,\t\t\t!Initial value of solar wind sound speed\n";
    print OUT "  bx_0 = 0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  by_0 = 0.0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  bz_0 = 0.0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  tzero=3000.0,\t\t\t!Secs of Initial solar wind conditions\n";
    print OUT "  by_coef=$by,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  bz_coef=$bz,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  bx_zero=${bx}e-5,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  GSM=.true.,\t\t\t!Perform calcs in GSM coords\n";
    print OUT "  SM=$sm,\t\t\t!Solar Wind file in SM coords\n";
    print OUT "  tilt_it=$tiltit,\t\t!Use fixed dipole tilt\n";
    print OUT "  iday=$iday,\t\t\t!Day and Month of fixed dipole tilt\n";
    print OUT "  time_of_day=$time_of_day,\t\t!UT of fixed dipole tilt\n";
    print OUT " &END\n";
    print OUT " &ION\n";
    print OUT "  ionmodel=$ionmodel,\t\t!Use empirical ionospheric model\n";
    print OUT "  S107=$f107,\t\t\t!10.7 Flux\n";
    print OUT "  hallmin=1.0,\t\t\t!Minimum value for Hall Conductivity\n";
    print OUT "  pedmin=2.0,\t\t\t!Minimum value for Pedersen Conductivity\n";
    print OUT "  SIGMA_RATIO=1.0,\t\t!Max Ratio of Hall to Ped Conductity\n";
    print OUT "  P_HARDY=2.24,\t\t\t!Empirical flux constant\n";
    print OUT "  C_HARDY=1.0,\t\t\t!Empirical flux constant\n";
    print OUT "  H_HARDY=1.333,\t\t!Empirical flux constant\n";
    print OUT "  ped0=$ped0,\t\t\t!Value of Ped if no ion model\n";
    print OUT "  iter = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  itmax = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  ERRMAX=2.E-3,\t\t\t!Error certerion in ellipitic solve\n";
    print OUT "  ion_skip=10,\t\t\t!Recalc Ionosphere every N steps\n";
    print OUT " &END\n";
    close(OUT);
}

sub writeinput1lp {
#This subroutine creates the INPUT1-???? files for the parallel LFM
#with the following arguments
  my ($name,$nrings,$naver,$bx,$by,$bz,
      $ionmodel,$f107,$ped0,$it,$start,$stop,$dump,$count) = @_;

  my($filename);
 
  $filename = sprintf ">INPUT1-%4.4d",$count;
  open(OUT,"$filename") || die "Cannot open $filename";
    print OUT " &GO\n";
    print OUT "  NSTART=$start,\t\t\t!Initial Step of this segment\n";
    print OUT "  NSTOP=$stop,\t\t\t!Final Step of this segment\n";
    print OUT "  NDUMP=$dump,\t\t\t!Frequency of dumps to HDF File\n";
    print OUT "  HDFBASE_IN=\'$name\',\t\t\t!Base name of input HDF File\n";
    print OUT "  HDFBASE_OUT=\'$name\',\t\t\t!Base name of output HDF File\n";
    print OUT "  RUN_Name=\'$name\',\t\t\t!Name identifier in HDF File\n";
    print OUT "/\n";
    print OUT " &RINGS\n";
    print OUT "  NRINGS=$nrings,\t\t\t!Number of averaging bins\n";
    print OUT "  NAVER=$naver,\t\t!Size of avg bins (must be nj in size)\n";
    print OUT "/\n";
    print OUT " &CONST\n";
    print OUT "  CA=1.1e8,\t\t\t!Speed of light for Borris Correction\n";
    print OUT "  CRNT=0.3,\t\t\t!Cournt Number\n";
    print OUT "/\n";
    print OUT " &SWIND\n";
    print OUT "  SWFROMFILE=.false.,\t\t\t !Get SW from HDF File\n";
    print OUT "/\n";
    print OUT " &ION\n";
    print OUT "  ionmodel=$ionmodel,\t\t!Use empirical ionospheric model\n";
    print OUT "  S107=$f107,\t\t\t!10.7 Flux\n";
    print OUT "  ped0=$ped0,\t\t\t!Value of Ped if no ion model\n";
    print OUT "  iter = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  itmax = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  nloop=1,\t\t\t!Recalc Ionosphere every N steps\n";
    print OUT "/\n";
    close(OUT);
}

sub writeinput1r {
#This subroutine creates the INPUT1-start and INPUT1-restart files
#with the following arguments
  my ($nrings,$naver,$bx,$by,$bz,$sm,$tiltit,$iday,$time_of_day,
      $ionmodel,$f107,$fed_alpha,$fed_beta,$fed_r,$ped0,$it) = @_;
 
  my (@restart,$restart,$val);
  @restart = (".true.",".false.");
  foreach $restart (@restart) {
    if ($val = $restart =~ /false/) {
      open(OUT,">INPUT1-start") || die "Cannot open INPUT-start";
    } else {
      open(OUT,">INPUT1-restart") || die "Cannot open INPUT-start";
    }
    print OUT " &GO\n";
    print OUT "  RSTRT=$restart,\t\t\t!Restart from previous HDF File\n";
    print OUT "  NSTART=0,\t\t\t!Initial Step of this segment\n";
    print OUT "  NSTOP=100,\t\t\t!Final Step of this segment\n";
    print OUT "  NDUMP=25,\t\t\t!Frequency of dumps to HDF File\n";
    print OUT "  ORDER=8,\t\t\t!Numerical order of calculation\n";
    print OUT "  NRINGS=$nrings,\t\t\t!Number of averaging bins\n";
    print OUT "  NAVER=$naver,\t\t!Size of avg bins (must be nj in size)\n";
    print OUT "  GEOQMU=0.30,\t\t\t!Strength of B field at pole\n";
    print OUT " &END\n";
    print OUT " &CONST\n";
    print OUT "  GAMMA=1.66666667,\t\t!Thermo gas constant\n";
    print OUT "  CA=1.1e8,\t\t\t!Speed of light for Borris Correction\n";
    print OUT "  CRNT=0.3,\t\t\t!Cournt Number\n";
    print OUT "  PDMB=4.,\t\t\t!PDM Noliner Switch Constant\n";
    print OUT "  AXFIX=0.5,\t\t\t!Normalized B Field Diff Coeff around Axis\n";
    print OUT " &END\n";
    print OUT " &SWIND\n";
    print OUT "  rho_start=2.25e-25,\n";
    print OUT "  c_start=1.5e6,\n";
    print OUT "  rho_0 = 1.125e-23,\t\t!Initial value of solar wind density\n";
    print OUT "  vx_0 = -4.0e7,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  vy_0 = 0.0,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  vz_0 = 0.0,\t\t\t!Initial value of solar wind velocity\n";
    print OUT "  c_0 = 3.5e6,\t\t\t!Initial value of solar wind sound speed\n";
    print OUT "  bx_0 = 0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  by_0 = 0.0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  bz_0 = 0.0,\t\t\t!Initial value of solar magnetic field\n";
    print OUT "  tzero=3000.0,\t\t\t!Secs of Initial solar wind conditions\n";
    print OUT "  by_coef=$by,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  bz_coef=$bz,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  bx_zero=${bx}e-5,\t\t\t!Coefficent for Bx fit\n";
    print OUT "  GSM=.true.,\t\t\t!Perform calcs in GSM coords\n";
    print OUT "  SM=$sm,\t\t\t!Solar Wind file in SM coords\n";
    print OUT "  tilt_it=$tiltit,\t\t!Use fixed dipole tilt\n";
    print OUT "  iday=$iday,\t\t\t!Day and Month of fixed dipole tilt\n";
    print OUT "  time_of_day=$time_of_day,\t\t!UT of fixed dipole tilt\n";
    print OUT " &END\n";
    print OUT " &ION\n";
    print OUT "  ionmodel=$ionmodel,\t\t!Use empirical ionospheric model\n";
    print OUT "  S107=$f107,\t\t\t!10.7 Flux\n";
    print OUT "  hallmin=1.0,\t\t\t!Minimum value for Hall Conductivity\n";
    print OUT "  pedmin=2.0,\t\t\t!Minimum value for Pedersen Conductivity\n";
    print OUT "  SIGMA_RATIO=1.0,\t\t!Max Ratio of Hall to Ped Conductity\n";
    print OUT "  FED_ALPHA=$fed_alpha,\t\t\t!Empirical flux constant\n";
    print OUT "  FED_beta=$fed_beta,\t\t\t!Empirical flux constant\n";
    print OUT "  FED_R=$fed_r,\t\t!Empirical flux constant\n";
    print OUT "  ped0=$ped0,\t\t\t!Value of Ped if no ion model\n";
    print OUT "  iter = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  itmax = $it,\t\t\t!Max num of iterations in ellipitic solve\n";
    print OUT "  ERRMAX=2.E-3,\t\t\t!Error certerion in ellipitic solve\n";
    print OUT "  ion_skip=10,\t\t\t!Recalc Ionosphere every N steps\n";
    print OUT " &END\n";
    close(OUT);
  }
}
sub writeinputdata {
#This subroutine creates the inputdata TING input files
  my ($count,$basename,$yr,$doy,$stdalone,$tingstep,
      $tingstartday,$tingstarthr,$tingstartmin,
      $tingstopday,$tingstophr,$tingstopmin,
      $tingdumpday,$tingdumphr,$tingdumpmin) = @_;
  my ($prev);
  my ($filename); 
   
  $filename = sprintf "inputdata-%4.4d",$count;
  open(OUT,">$filename") || die "Cannot open $filename";
  $prev = $count - 1; 
  print OUT "$basename\n";
  print OUT "C\n";
  print OUT "Script generated by mkcmitscripts.pl\n";
  print OUT "$yr\n";
  print OUT "$doy\n";
  print OUT "\n";
  print OUT "eqix\n";
  print OUT "$stdalone\n";
  print OUT "$tingstep\n";
  print OUT "$tingstartday,$tingstarthr,$tingstartmin\n";
  if ($count == 0) {
    print OUT "y\n";
  } else {
    print OUT "n\n";
  }
  print OUT "$basename-${prev}01.dat\n";
  print OUT "$tingstartday,$tingstarthr,$tingstartmin\n";
  print OUT "$basename-$count\n";
  print OUT "$tingstopday,$tingstophr,$tingstopmin\n";
  print OUT "$tingdumpday,$tingdumphr,$tingdumpmin\n";
  print OUT "\n";
  print OUT "19000.0, 3000.0, 5000.0, 9000.0, 5000.0\n";
  print OUT "10.3, 8.5, 7.7, 2.2, 7.4\n";
  print OUT "0\n";
  print OUT "0\n";
  print OUT "\n";
  print OUT "\n";
  close(OUT);

}

sub writedinput {
# This one creates the dinput.dat file with the f107 argument
  my ($f107) = @_;
  
  open(OUT,">dinput.dat") || die "Cannot open dinput.dat\n";
  print OUT "      1\n";
  print OUT "      1  $f107                                                               F107\n";
  print OUT "      1\n";
  print OUT "      1  $f107                                                              F107A\n";
  print OUT "      1     0                                                             OLDALF\n";
  print OUT "      1                                                                        1\n";
  print OUT "      1  11.0                                                              POWER\n";
  print OUT "      1                                                                        2\n";
  print OUT "      1  60.0                                                              CTPOT\n";
  print OUT "      1                                                                        3\n";
  print OUT "      1   0.0                                                              IMFBY\n";
  print OUT "      0                                                                        4\n";
  print OUT "      1   0.0                                                                 R1\n";
  print OUT "      0                                                                        5\n";
  print OUT "      1   0.0                                                              PHIDM\n";
  print OUT "      0                                                                        6\n";
  print OUT "      1   0.0                                                              PHIDP\n";
  print OUT "      0                                                                        7\n";
  print OUT "      1   0.0                                                              PHINM\n";
  print OUT "      0                                                                        8\n";
  print OUT "      1   0.0                                                              PHINP\n";
  print OUT "      0                                                                        9\n";
  print OUT "      1   0.0                                                             ARADPN\n";
  print OUT "      0                                                                       10\n";
  print OUT "      1   0.0                                                             ARADNS\n";
  print OUT "      0                                                                       11\n";
  print OUT "      1   0.0                                                             OFFAPN\n";
  print OUT "      0                                                                       12\n";
  print OUT "      1   0.0                                                             OFFANS\n";
  print OUT "      0                                                                       13\n";
  print OUT "      1   0.0                                                            DKOFAPN\n";
  print OUT "      0                                                                       14\n";
  print OUT "      1   0.0                                                            DKOFANS\n";
  print OUT "      0                                                                       15\n";
  print OUT "      1   0.0                                                             PHIDPN\n";
  print OUT "      0                                                                       16\n";
  print OUT "      1   0.0                                                             PHIDNS\n";
  print OUT "      0                                                                       17\n";
  print OUT "      1   0.0                                                             PHINPN\n";
  print OUT "      0                                                                       18\n";
  print OUT "      1   0.0                                                             PHINNS\n";
  print OUT "      0                                                                       19\n";
  print OUT "      1   0.0                                                             PCEPPN\n";
  print OUT "      0                                                                       20\n";
  print OUT "      1   0.0                                                             PCEPNS\n";
  print OUT "      0                                                                       21\n";
  print OUT "      1   0.0                                                             PCENPN\n";
  print OUT "      0                                                                       22\n";
  print OUT "      1   0.0                                                             PCENNS\n";
  print OUT "      0                                                                       23\n";
  print OUT "      1   0.0                                                            DISPCPN\n";
  print OUT "      0                                                                       24\n";
  print OUT "      1   0.0                                                            DISPCNS\n";
  print OUT "      0                                                                       25\n";
  print OUT "      1   0.0                                                             DISPPN\n";
  print OUT "      0                                                                       26\n";
  print OUT "      1   0.0                                                             DISPNS\n";
  print OUT "      0                                                                       27\n";
  print OUT "      1   0.0                                                             OFFCPN\n";
  print OUT "      0                                                                       28\n";
  print OUT "      1   0.0                                                             OFFCNS\n";
  print OUT "      0                                                                       29\n";
  print OUT "      1   0.0                                                                 E1\n";
  print OUT "      0                                                                       30\n";
  print OUT "      1   0.0                                                                 E2\n";
  print OUT "      0                                                                       31\n";
  print OUT "      1   0.0                                                                 H1\n";
  print OUT "      0                                                                       32\n";
  print OUT "      1   0.0                                                                 H2\n";
  print OUT "      0                                                                       33\n";
  print OUT "      1   0.0                                                               ROTE\n";
  print OUT "      0                                                                       34\n";
  print OUT "      1   0.0                                                               ROTH\n";
  print OUT "      0                                                                       35\n";
  print OUT "      1   0.0                                                               TWA6\n";
  print OUT "      0                                                                       36\n";
  print OUT "      1   0.0                                                              TWA21\n";
  print OUT "      0                                                                       37\n";
  print OUT "      1   0.0                                                              ALFA1\n";
  print OUT "      0                                                                       38\n";
  print OUT "      1   0.0                                                              ALFA2\n";
  print OUT "      0                                                                       39\n";
  print OUT "      1   0.0                                                            DKOFCNH\n";
  print OUT "      0                                                                       40\n";
  print OUT "      1   0.0                                                            DKOFCSH\n";
  print OUT "      0                                                                       41\n";
  print OUT "      1   0.0                                                           THETA0NH\n";
  print OUT "      0                                                                       42\n";
  print OUT "      1   0.0                                                           THETAOSH\n";
      
  close(OUT);
}

sub writeinputrcm {
#This subroutine creates the INPUT-RCM-start and INPUT-RCM-restart files
#with the following arguments
  my ($rcmyr,$rcmmon,$rcmday,$rcmhr,$rcmmin,$rcmsec,
      $rcmstep) = @_;
  my (@restart,$restart,$val);
  @restart = (".true.",".false.");
  foreach $restart (@restart) {
  if ($val = $restart =~ /false/) {
    open(OUT,">INPUT-RCM-start") || die "Cannot open INPUT-start";
  } else {
    open(OUT,">INPUT-RCM-restart") || die "Cannot open INPUT-start";
  }
  print OUT " &RCMinput\n";
  print OUT "  iRCMYr=$rcmyr,\t\t\t! Year to start coupling with RCM\n";
  print OUT "  iRCMMon=$rcmmon,\t\t\t! Month to start coupling with RCM\n";
  print OUT "  iRCMDay=$rcmday,\t\t\t! Day to start coupling with RCM\n";
  print OUT "  iRCMHr=$rcmhr,\t\t\t! Hour to start coupling with RCM\n";
  print OUT "  iRCMMin=$rcmmin,\t\t\t! Minute to start coupling with RCM\n";
  print OUT "  iRCMSec=$rcmsec,\t\t\t! Second to start coupling with RCM\n";
  print OUT "  rRCMDelta=$rcmstep,\t\t\t! Seconds between LFM/RCM exchanges\n";
  print OUT "  RCM_restart=$restart,\t\t\t! Restarting in RCM interval\n";
  print OUT "  bleed_rate=1.0,\t\t\t! Rate to bleed information into LFM\n";
  print OUT "  RCM_bleed_type_scheme=2,\t\t\t! Which bleed scheme to use\n";
  print OUT " &END\n";
  close(OUT);
  }
}

sub writeinputmf {
#This subroutine creates the INPUT-MF-start file
#with the following arguments
  my ($h_mass,$o_mass,$sw_ratio,$e_frac,$h_frac,$o_frac,$hp_frac,$op_frac,
      $singtomf) = @_;
  open(OUT,">INPUT-MF") || die "Cannot open INPUT-MF";
  print OUT " &MFLFM\n";
  print OUT "  h_mass=$h_mass,\t\t\t! H species mass\n";
  print OUT "  o_mass=$o_mass,\t\t\t! O species mass\n";
  print OUT "  sw_ratio=$sw_ratio,\t\t\t! Ratio of species in SW\n";
  print OUT "  single_to_multi=$singtomf,\t! Convert Single to Multi\n";
  print OUT "  electron_frac=$e_frac,\t\t! Fraction of Electrons\n";
  print OUT "  hfrac=$h_frac,\t\t\t! Fraction of H species\n";
  print OUT "  ofrac=$o_frac,\t\t\t! Fraction of O species \n";
  print OUT "  hPfrac=$hp_frac,\t\t\t! Fraction of P from H species\n";
  print OUT "  oPfrac=$op_frac,\t\t\t! Fraction of P from O species\n";
  print OUT " &END\n";
  close(OUT);
  }


sub writeinput1xml {
#This subroutine creates the INPUT1-???? files for the parallel LFM
#with the following arguments
  my ($nstart,$nstop,$ndump,
      $startdatetime,$stopdatetime,$timedumpinterval,
      $hdfbasein,$hdfbaseout,$runname,
      $ca,$swfromfile,$bxc,$byc,$bzc,
      $ionmodel,$s107,$ped0,$count) = @_;

  my($filename);
 
  my $gen = XML::Generator->new(':pretty');
  $filename = sprintf ">INPUT1-%4.4d.xml",$count;
  open(OUT,"$filename") || die "Cannot open $filename";
  select OUT; 
  print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  print "<modules>\n";
  if ($ndump > 0) {
  print $gen->module(
           $gen->name("STEPS"),
           $gen->meta("Run parameters"),
           $gen->variable(
              $gen->name("NSTART"),
              $gen->meta("Initial Step"),
              $gen->data("$nstart")
           ),
           $gen->variable(
              $gen->name("NSTOP"),
              $gen->meta("Final Step"),
              $gen->data("$nstop")
           ),
           $gen->variable(
              $gen->name("NDUMP"),
              $gen->meta("Dump Interval"),
              $gen->data("$ndump")
           )
        );
  print "\n";
  } else {
  print $gen->module(
           $gen->name("UTIO"),
           $gen->meta("UT Information"),
           $gen->variable(
              $gen->name("START_DATETIME"),
              $gen->meta("UT Time of start"),
              $gen->data("$startdatetime"),
              $gen->format("YYYY MM DD HH MM SS"),
           ),
           $gen->variable(
              $gen->name("STOP_DATETIME"),
              $gen->meta("UT Time of stop"),
              $gen->data("$stopdatetime"),
              $gen->format("YYYY MM DD HH MM SS"),
           ),
           $gen->variable(
               $gen->name("TIME_DUMP_INTERVAL"),
               $gen->meta("Time in seconds between dumps"),
               $gen->data("$timedumpinterval"),
          )
        );
  print "\n";
  }
  print $gen->module(
           $gen->name("GO"),
           $gen->meta("Run parameters"),
           $gen->variable(
              $gen->name("HDFBASE_IN"),
              $gen->meta("Base name for input HDF files "),
              $gen->data("$hdfbasein")
           ),
           $gen->variable(
              $gen->name("HDFBASE_OUT"),
              $gen->meta("Base name for output HDF files "),
              $gen->data("$hdfbaseout")
           ),
           $gen->variable(
              $gen->name("RUN_NAME"),
              $gen->meta("Run string placed in HDF files"),
              $gen->data("$runname")
           )
         );
  print "\n";
  print $gen->module(
           $gen->name("CONST"),
           $gen->meta("Physical Constants"),
           $gen->variable(
              $gen->name("CA"),
              $gen->meta("c for Borris correction [cm/s]"),
              $gen->data("$ca")
           )
          );
  print "\n";
  print $gen->module(
           $gen->name("SWIND"),
           $gen->meta("Solar wind parameters"),
           $gen->variable(
              $gen->name("SWFROMFILE"),
              $gen->meta("Get solar wind info from HDF file"),
              $gen->data("$swfromfile")
           ),
           $gen->variable(
              $gen->name("BX_ZERO"),
              $gen->meta("Bx = Bx_zero + BY_COEF*BY + BZ_COEF*BZ"),
              $gen->data("$bxc")
           ),
           $gen->variable(
              $gen->name("BY_COEF"),
              $gen->meta("Bx = Bx_zero + BY_COEF*BY + BZ_COEF*BZ"),
              $gen->data("$byc")
           ),
           $gen->variable(
              $gen->name("BZ_COEF"),
              $gen->meta("Bx = Bx_zero + BY_COEF*BY + BZ_COEF*BZ"),
              $gen->data("$bzc")
           )
         );
  print "\n";
  print $gen->module(
           $gen->name("ION"),
           $gen->meta("Ionospheric parameters"),
           $gen->variable(
              $gen->name("IONMODEL"),
              $gen->meta("Use ionospheric Precip Model"),
              $gen->data("$ionmodel")
           ),
           $gen->variable(
              $gen->name("S107"),
              $gen->meta("F 10.7 Flux"),
              $gen->data("$s107")
           ),
           $gen->variable(
              $gen->name("PED0"),
              $gen->meta("Pedersen Conductance"),
              $gen->data("$ped0")
           )
         );
  print "\n";
  print "</modules>\n";
  close(OUT);
}
1;
__END__
