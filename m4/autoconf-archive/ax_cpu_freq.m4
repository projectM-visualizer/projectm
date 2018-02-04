# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_cpu_freq.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CPU_FREQ
#
# DESCRIPTION
#
#   Compute the CPU frequency and define CPU_FREQ accordingly.
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

AC_DEFUN([AX_CPU_FREQ],
[AC_REQUIRE([AC_PROG_CC])
 AC_LANG_PUSH([C++])
 AC_CACHE_CHECK(your cpu frequency, ax_cpu_freq,
 [AC_RUN_IFELSE([AC_LANG_PROGRAM([
#include <iostream>
#include <sys/time.h>
#include <fstream>
using namespace std;

static __inline__ unsigned long long int rdtsc()
{
  unsigned long long int x;
  __asm__ volatile (".byte 0x0f, 0x31":"=A" (x));
  return x;
}

static float estimate_MHz(long sleeptime = 250000)
{
  struct timezone tz;
  struct timeval tvstart, tvstop;
  unsigned long long int cycles[[2]];
  float microseconds;
  double freq = 1.0f;

  memset(&tz, 0, sizeof(tz));

  gettimeofday(&tvstart, &tz);
  cycles[[0]] = rdtsc();
  gettimeofday(&tvstart, &tz);

  usleep(sleeptime);

  gettimeofday(&tvstop, &tz);
  cycles[[1]] = rdtsc();
  gettimeofday(&tvstop, &tz);

  microseconds = (tvstop.tv_sec - tvstart.tv_sec) * 1000000 +
    (tvstop.tv_usec - tvstart.tv_usec);

  return (float) (cycles[[1]] - cycles[[0]]) / (microseconds / freq);
}

static float average_MHz(int tries = 2)
{
  float frequency = 0;

  for (int i = 1; i <= tries; i++)
    frequency += estimate_MHz(i * 150000);

  if (tries > 0)
    return frequency / (float) tries;
  else
    return 0;
}
], [
    ofstream of("conftest_cpufreq");
    if(of.is_open())
      of << average_MHz();
    else
      return 1;

    of.close()
])],
     [ax_cpu_freq=`cat conftest_cpufreq`; rm -f conftest_cpufreq],
     [ax_cpu_freq=unknow; rm -f conftest_cpufreq]
 )])
AC_LANG_POP([C++])

  AC_DEFINE_UNQUOTED([CPU_FREQ], ${ax_cpu_freq}, [The cpu frequency (in MHz)])
])
