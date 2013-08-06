$! --------------------------------------------------------------------------
$! For making GETOPT.OBJ on VMS if you don't have MMS.
$! --------------------------------------------------------------------------
$!
$! $Id: make.com 510 1993-05-03 21:27:09Z chouck $
$
$ ccc := cc /opt/nodebug/nolist
$
$ ccc GETOPT.C
$
