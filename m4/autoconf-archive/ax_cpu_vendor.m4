# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_cpu_vendor.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CPU_VENDOR
#
# DESCRIPTION
#
#   Find your CPU's vendor by requesting cpuid and define "ax_cpu_vendor"
#   accordingly. This macro depends on AX_GCC_X86_CPUID.
#
# LICENSE
#
#   Copyright (c) 2008 Christophe Tournayre <turn3r@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_CPU_VENDOR],
[
 AC_REQUIRE([AX_GCC_X86_CPUID])
 AX_GCC_X86_CPUID(0x0)

 AC_CACHE_CHECK(for the processor vendor, ax_cpu_vendor,
 [
   vendor=`echo $ax_cv_gcc_x86_cpuid_0x0 | cut -d ":" -f 2`

   case $vendor in
     756e6547*)
       ax_cpu_vendor="Intel"
       ;;
     68747541*)
       ax_cpu_vendor="AMD"
       ;;
     69727943*)
       ax_cpu_vendor="Cyrix"
       ;;
     746e6543*)
       ax_cpu_vendor="IDT"
       ;;
     646f6547*)
       ax_cpu_vendor="Natsemi Geode"
       ;;
     52697365*)
       ax_cpu_vendor="Rise"
       ;;
     65736952*)
       ax_cpu_vendor="Rise"
       ;;
     20536953*)
       ax_cpu_vendor="SiS"
       ;;
     *)
       ax_cpu_vendor="Unknown"
       ;;
   esac
 ])
])
