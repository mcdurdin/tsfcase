Text Services Framework Case Sample Readme
==========================================

This sample is based on tsfcase from the Windows 7 SDK examples,
with unnecessary example code stripped out.
https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/Win7Samples/winui/tsf/tsfcase

The example provides a repro for the Windows Feedback reported at 
https://aka.ms/AA30g4p, where a TIP registered against Amharic, Tigrinya or
Sinhala will not be successfully activated in most situations, on Windows 10 1803 
or later. A detailed report in the Keyman project (3rd party TSF TIP IME) is
at https://github.com/keymanapp/keyman/issues/1285. The behaviour may not present
identically between 1803 - 1909.

The code implements a simple text service which transforms a-z, A-Z 
to circled forms (U+24B6-U+24CF, U+24D0-U+24E9).

Ctrl+F is a preserved key which inserts an inverted circled zero (U+24FF).

The TIP is registered with English (US) and Amharic (Ethiopia).

Target audience is text service writers.

Installation
==========================================

  regsvr32 tsfcase.dll

from the command line to register it with the system.

This is currently only a 32-bit example but that does
not impact the repro.

Repro Steps under Windows 10 1909
==========================================
Load 32-bit notepad or any other 32-bit app. When selecting
English (US) + Case IM, the TIP is loaded successfully and
you can observe the transforms.

If you select Amharic, however, the TIP will not be loaded.
Some parts of the Windows UI will show the TIP selected, but
the default Amharic IME will instead be loaded.

If you enable the class language bar and attempt to select
the TIP through that interface, it never loads.