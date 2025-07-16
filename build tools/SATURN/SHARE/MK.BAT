@echo off
..\sh\bin\shc -sub=shc.sub fss.c>fss.err
..\sh\bin\shc -sub=shc.sub sddrvs.c>sddrvs.err
..\sh\bin\shc -sub=shc.sub smpmap.c>smpmap.err
dir *.err