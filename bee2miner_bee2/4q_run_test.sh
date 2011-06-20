#!/bin/sh

if test "$1" = "--help" ; then
    echo "Usage 4q_run_test.sh [-timeout 25] program.dat.iss <TCC|CACHED|DEFAULT> <mc_config> <args to resetnio>";
   #echo "Usage run_test.sh program.dat.iss <TCC|CACHED|DEFAULT> <mc_config> <args to resetnio>";
    exit;
fi

# Pass timeout value to sm_resetnio via environment variable (is this too awful?)
if test "$1" = "-timeout" ; then
  shift; timeout=$1; shift;
 #echo "FOO print run_test found timeout $timeout"
  export DOIO_NCHECKS=$timeout
fi

########################################################################
# Unpack the args

arg1=$1; shift
arg2=$1; shift
arg3=$1; shift

########################################################################
# Make up to $nretries attempts before giving up.

nretries=1
for attempt in $(seq 1 1 $nretries); do

  echo; echo "Attempt $attempt of $nretries"; echo

  ########################################################################
  # Make sure no competing "sm_resetnio or 4q_resetnio" processes are already running

  pid=$(ps ax | grep -v grep | grep resetnio | awk '{print $1}')
  if [ $pid > 0 ] ; then
    echo Found competing process $pid
    ps ax | grep -v grep | grep resetnio
    echo Killing competing process $pid
    kill -9 $pid
  fi

  ########################################################################
  # De-assert PreventReset, assert and de-assert Reset
  #4q_rw_reg W PREVENTRESET_N 1
  
  # Reset ALL 4 Quads
  echo "Resetting all Quads"
  4q_rw_reg W RESET_N 0 F
  4q_rw_reg W RESET_N 1 F
 
  ########################################################################
  # Clean up the offchip communication state (the buffers are reset
  # in the step above)
  #
  echo "Resetting Offchip Buffers"
  4q_rw_reg W OFFCHIP_STATE 0 F
  
  ########################################################################
  # Configure the memory translators.
  4q_configure_mem $arg2 $arg3;
  if test "$?" = "1"; then
    echo "configure_mem failed. Aborting."; exit;
  fi

  ########################################################################
  # Load the program
  4q_load_program $arg1 $arg2 $arg3; 
  if test "$?" = "1"; then
    echo "load_program failed. Aborting."; exit;
  fi

  ########################################################################
  # De-assert preventreset (redundant), assert reset (probably don't need any of this yet).
  #4q_rw_reg W PREVENTRESET_N 1
  
  4q_rw_reg W RESET_N 0 F
  
  # De-assert reset.
  4q_rw_reg W RESET_N 1 F
  
 
  ########################################################################
  # On failure / retry, reload memory configuration.
  # Memory configuration should persist across runs.
  # (Just always do this now)
  #if test $attempt '>' 1; then

  # Configure the MC
  #echo "Configuring Memory Controller(s)"
  #4q_cfg_mc $arg3
  #  if test "$?" = "1"; then
  #    echo "4q_cfg_mc failed. Aborting."; exit;
  #    exit;
  #  fi
  #fi

  #/bin/rm stdout.log

  #Run the program!
  4q_resetnio $arg3 "$@"
  if test "$?" = "0"; then
    echo "4q_resetnio completed successfully."; exit;
  fi

  echo ""
  echo "4q_resetnio was unsuccessful."

done
echo; echo "Gave up after $nretries tries."; echo
