#!perl -w

###############################################################################
##                                                                           ##
##    Copyright (c) 1995 - 2004 by Steffen Beyer.                            ##
##    All rights reserved.                                                   ##
##                                                                           ##
##    This package is free software; you can redistribute it                 ##
##    and/or modify it under the same terms as Perl itself.                  ##
##                                                                           ##
###############################################################################

use strict;

use ExtUtils::MakeMaker;

WriteMakefile(
    'NAME'              => 'LTR::Inputs',
    'VERSION_FROM'      => 'Inputs.pm',
#   ($] >= 5.005 ?
#       ('ABSTRACT'     => 'Report errors from perspective of caller of a "clan" of modules',
#        'AUTHOR'       => 'Steffen Beyer (sb@engelschall.com)') : ()),
#   ($] >= 5.005 && $^O eq 'MSWin32' && $Config{archname} =~ /-object\b/i ?
#       ('CAPI'         => 'TRUE') : ()),
    'dist'              => { COMPRESS => "gzip -9", SUFFIX => "gz" }
);

__END__

