Start of Document==============================================================

                   SNASM2 Development System Release Notes for 18 March, 1996

===============================================================================

This is the README for the SNASM2 Development System. This
documented contains information about new features, changes to existing
features and bug fixes for the SNASM2 tools. This document may also contain
last-minute information that has yet to be included in the SNASM2 User
Manuals.

Note: This README applies to the entire SNASM2 Development System and
supercedes any README files that you may have downloaded with individual
programs from our BBS.




===============================================================================
                                                                   0.0 CONTENTS
-------------------------------------------------------------------------------

1.0 SUMMARY

2.0 THIS RELEASE

2.1 The Debugger (V2.11a, 18 March 1996)
    2.1.1    What's New
    2.1.3    What's Changed
    2.1.4    What's Fixed

2.2 The Assembler (Version 2.10d, 06 December 1995)
    2.2.1    What's New
    2.2.2    What's Changed
    2.2.3    What's Fixed

2.3 Utilities

3.0 PREVIOUS RELEASES
    3.1    The Debugger (V2.10e, 21 December 1995)
    3.2    The Debugger (V2.10d, 13 December 1995)
    3.3    The Debugger (V2.10c, 30 October 1995)

4.0 GENERAL INFORMATION
    4.1    The Configuration File (SNBUG.CFG)
    4.2    Support for GNU C++ Function Name Demangling
    4.3    The Fileserver library (LIBCROSS.A)

5.0 TARGET  SPECIFIC INFORMATION

6.0 LAST MINUTE INFORMATION



===============================================================================
                                                         1.0 SUMMARY OF CHANGES
-------------------------------------------------------------------------------

Debugger Files  Description                     New?     Version #
--------------  ---------------------           ----     ---------
SNBUG32X.EXE    32X Debugger                    Yes      2.11a
SNBUGSAT.EXE    Saturn Debugger                 Yes      2.11a
MARSMON.BIN     32X Monitor                     No       2.10f
SATMON.BIN      Saturn Monitor                  No       2.10f
SNBUG.CFG       Default Session File            No       2.10f
START.PRG       Startup Script File             No       2.10f
LIBCROSS.A      Fileserver library              No       2.10f


DBVERSN.TXT     Debugger Readme                 No       Replaced by this file
DEMANGLE.TXT    C++ Support Readme              No       Replaced by this file
LIBCROSS.TXT    Fileserver Readme               No       Replaced by this file
README.32X      32X Debugger Readme             No       Replaced by this file
README.SAT      Saturn Debugger Readme          No       Replaced by this file
README.NOW      General Readme                  No       Replaced by this file
SEARCH.TXT      Search Facility Readme          No


Assembler Files Description                     New?    Version #
--------------- ----------------------          ----    -----------
SNASMSH2.EXE    SH2 Assembler                   No      2.10d
SNASM68K.EXE    68000 Assembler                 No      2.10d
FIXES.TXT       Assembler Readme                No      Replaced by this file


Utilities       Description                     New?    Version #
---------       ---------------------------     ----    ---------
SNMAKE.EXE      Make Utility                    No      1.40n
SNLIB.EXE       Librarian Utility               No      1.06
S2G.EXE         SNASM-to-GNU COFF Converter     No      2.08
32XLOAD.EXE     32X COFF Downloader             No      1.0a
SATLOAD.EXE     Saturn COFF Downloader          No      1.0a



===============================================================================
                                                               2.0 THIS RELEASE
-------------------------------------------------------------------------------

2.1 The Debugger (V2.11a, 18 March 1996)
    ============


2.1.1   What's New in the Debugger
        ===========================

2.1.1.a COFF Loading Speed Improved
    	---------------------------

    	A COFF with a larger number of symbols will load much faster.


2.1.3   What's Changed in the Debugger
        ==============================
        There are no changes to existing features for this release.


2.1.4   What's Fixed in the Debugger
        =============================

2.1.4.a	Page Dump During Structure Browse Fixed
	    ---------------------------------------

    	The irregular case of a structure or class having a data member
	of type "void" caused the debugger to crash.  The 'value' of such an
	object is displayed as "Illegal Object Type!"; the debugger no longer
	crashes.

2.1.4.b	Source and Mixed Windows Find Source Files Correctly
	----------------------------------------------------

	With some build directory layouts, problems with finding source files
	were seen.  These are now fixed.


2.1.4.c	Problem with Structure Browse of Pointers Fixed
	-----------------------------------------------

	When an expanded pointer was scrolled off the top of the watch
	window, the wrong data was displayed.  This is now resolved.

2.1.4.d	Local Variables Window Internal Error Problem Fixed
	---------------------------------------------------

	When a local variable window was open, tracing out of a function
	with no local variables was reported as an internal error.
	This has been resolved.

2.1.4.e	Assertion During Structure Browse Not Triggered
	-----------------------------------------------

	The bug that caused an assertion failure when closing two expanded
	pointers of the same type has been fixed.

2.1.4.f	Multiple Instances Structure Browses of an Array Fixed
	------------------------------------------------------

 	Browsing the same array twice or more would cause the debugger to
 	crash when one of the instances was deleted.  This is now corrected.


2.2     The Assembler (Version 2.10d, 06 December 1995)
        ===============================================

2.2.1   What's New
        ==========
        There are no new features for this release.


2.2.2   What's Changed
        ==============

2.2.2.a DS Directive [CD-I only]
        ------------------------
        Space allocated with DS can now go inline or in a separate section.

2.2.2,b Error Messages [All Platforms]
        ------------------------------
        Error messages now have a number unique identifying number.
        Typographical corrections were made to the text of several error
        messages.


2.2.3   What's Fixed
        ============

2.2.3.a Module Boundaries [CD-I only]
	-----------------------------
        A problem with bad fixups across module boundaries has been fixed.



2.3     Utilities
        ========
        There are no changes to the utilities for this release.

===============================================================================
                                                          3.0 PREVIOUS RELEASES
-------------------------------------------------------------------------------

3.1.    The Debugger (V2.10f, 15 January 1996)
        =======================================


3.1.1   What's New in the Debugger
        ===========================

3.1.1.a Basic C++ Tutorial for Saturn Target System
        -------------------------------------------
        This tutorial introduces features in the debugger that have been
        added to provide support  for C++ programs.

        The tutorial files are:
                MAKEFILE        Basic MAKEFILE
                EXAMP.CPP       The C++ source file
                SL.COF          The pre-compiled COFF of EXAMP.CPP
                TUTORIAL.TXT    The tutorial file


3.1.1.b Tracing in a Mixed Window now Supports C++ Templates
        ----------------------------------------------------
        When tracing, the default behaviour of the cursor display when a
        processor exception occurs has been changed to support C++ Templates.

        If a processor exception occurs when tracing in the active
        Source Region of a Mixed Window, both Source and Disassembly regions
        now highlight the current PC instruction. This over-rides the default
        synchronization mechanism between the active and inactive region.

        This change has been made to ensure the correct display when, for
        example, tracing C++ Template Functions in the active Source region. I
        fa processor exception occurs the Source region will display
        the Template Definition and the Disassembly region will display the
        corresponding 'actual' instructions executed for each Template
        function type. Note that Template function names are not currently
        demangled.

        Previously, when tracing in this manner the inactive
        Disassembly region would *always* be synchronized to the Source region
        using the Debugging information. This behaviour would always show
        instructions for the first Template definition.

        Note that as soon as any other event occurs after a trace event (such
        as scrolling through the active region) the usual synchronization
        mechanism, using Debug information, will resume.


3.1.1.c Support for Long Pathnames
        --------------------------
        The debugger now supports long pathnames, inline with the latest
        version of the GNU SH2 compiler.


3.1.3   What's Changed in the Debugger
        ==============================
        There are no changes to existing features for this release.


3.1.4   What's Fixed in the Debugger
        =============================

3.1.4.a Fixes to the Structure Browse Facility
        --------------------------------------
        A number of bugs in the structure browsing facility have been
        fixed:

        - Arrays of type "Not available" now expand correctly; previously,
          expanding such arrays would cuase the debugger to crash. Arrays of
          this type now expand correctly, with elements having the same address
          but different indices.

        - Parameters in registers now show the containing register's name.
          Previously this information was not displayed.

        - In certain cases variables of the same name and type as others in
          a file would be displayed with the wrong data and/or type details.
          This is now fixed.


3.1.4.b Display of Source File Errors in the Source and Code Windows
        -------------------------------------------------------------
        Previously, the reporting of certain source file errors resulted in the
        information dialog being displayed repeatedly. This has now been
        fixed.


3.2.    The Debugger (V2.10e, 21 December 1995)
        =======================================

3.2.1   What's New
        ==========
        There are no new features for this release.

3.2.2
        What's Changed
        ==============
        There are no new changes for this release.


3.2.3   What's Fixed
        ============

3.2.3.a Hex. Dump To File (Memory Window) and Disassemble to File
        ---------------------------------------------------------
        Previously, data that displayed correctly on screen would contain
        a regularly repeating row of corrupt data in the Hex. dump/Disassembly
        in the capture file. This has now been fixed.


3.2.3.b Pointers in Register Variables now Browse Correctly
        ----------------------------------------------------
        Previously, pointer variables held in registers where an extra level
        of indirection was introduced would not browse correctly. This has now
        been fixed.



3.3     The Debugger (V2.10d, 13 December 1995)
        =======================================

3.3.1   What's New
        ==========

3.3.1.a Timeout command-line Option [-Exx] [All platforms]
	--------------------------------------------------
	A new command-line option [-Exx] has been added to allow user
	specification of the maximum retry timeout (in seconds) when
	communicating with the target machine. This has been implemented
	specifically for platforms which 'poll' rather than 'NMI' the target
	device but works on all target platforms.

	On these targets (such as CD-I), it is possible for a CD-ROM
	read or other transfer to continue uninterrupted until completion,
	often causing the poll mechanism in the Debugger to timeout. The
	timeout option allows such conditions to be avoided by setting a
	maximum length of time before timing out (the default is 1 second).

        This feature was originally introduced in v2.10z, 20 November 1995
        (Experimental Release).

3.3.1.b Hardware Memory Access Breakpoints [Saturn and 32X only] - MWT
        --------------------------------------------------------------
        Three new breakpoint types have been introduced into the Debugger:
	a 'Data Access' breakpoint, a 'Data Value' breakpoint and an
	'Advanced' breakpoint, where the development environment (Debug
	Hardware/target monitors etc.) permits. The Watch and Memory
	Debugger windows now have breakpointing capability in addition to Code
	style windows and of course the Breakpoint window.

        Code style windows only highlight instructions for which 'Execution'
	breakpoints have been defined. Watch and Memory windows only show
	memory access breakpoints (including 'Advanced').
	The Breakpoint window shows all user defined breakpoints of all types.

	All these types, along with the default 'Execution Breakpoint', may be
	manually specified in a Breakpoint window at the type selection dialog
	on creation of a new breakpoint. The Breakpoint window now displays
	additional info. relating to the new types, as 'EXEC' for execution
	usually specified from within a code style window, 'WAXS' for memory
	access of a given address and 'WDAT' for memory access using a
	read/write of a particular value to/from a given address usually
	specified in a Watch window or Memory window, and 'ADVN' for a user
	defined 'Advanced' configuration breakpoint as discussed below.

	The Debugger's Watch, Memory and Breakpoint windows now allow the use
	of 'F5' and 'Ctrl+F5' to define and configure a breakpoint (same
	mechanism as in Code windows). The mouse click in the left hand margin
	of a Watch window also toggles a breakpoint as for a Code window. The
	Watch window now has a margin of 4 characters. When a '.' is
	displayed, then a breakpoint may be placed on the watch expression
	(expanded or collapsed entries supported).

	The breakpoints toggled in a Watch and Memory window will default to
	the settings required to trap a :- data access of a read/write to the
	specified address, using an data access width specified by the
	variable type information from a Watch window (or ANY where not
	determinable), or, in the case of the Memory window, the default data
	access width will be ANY (a Byte, Word or Long) access width.

	For example, in a Watch window, if you place a breakpoint on an
	element of a expanded array being browsed, and it is defined as an
	array of longs, then the watchpoint will be defined for the address of
	the long within the array, a data access of Read/Write using a LONG
	word access.  This is a Data Access breakpoint. The access method
	(Read/Write) and access size (BYTE/WORD/LONG or ANY) may be modified
	in the dialog with additional 'Advanced' settings available.

	In a Memory window displaying with the 'Long' format specifier, a
	breakpoint will be defined at the address of the first byte of the
	group of bytes making up the long within which the cursor is
	displayed, and data access of Read/Write using a ANY access size.
	As an ANY access size can includes all 4 bytes when access as
	Byte/Word/Long, all 4 are highlighted as affected.
	To toggle or configure the breakpoint it may be necessary to change
	the display mode so that all bytes affected by the breakpoint are in
	the same group, e.g. the 4 bytes are displayed as a LONG. This helps
	the Debugger find the original address of the breakpoint from the bytes
	affected by it.

	Note that the breakpoint's access width works on the address boundary,
	e.g. a breakpoint defined at address 0x6000003 using LONG access width,
	will trap an access at address 0x6000000 as a LONG. If the access size
	where ANY, then the break occurs on an access at address 0x6000000 as
	a LONG, 0x6000002 as a WORD and 0x6000003 as a BYTE. The Memory window
	displays *all* the bytes affected in this manner.

	The 'Data Access' breakpoint may be modified to a 'Data Value'
	Breakpoint by selected 'Include Data Bus Conditions' check box. This
	will allow entry of a data value and a mask value, which will form
	additional conditions for the breakpoint, i.e. a read/write of a
	particular value or values (using the mask) to an address as a
	BYTE/WORD/LONG access etc. N.B. for Data Value breakpoints,
	BYTE/WORD/LONG must be specified, ANY is not valid. The default of
	BYTE access will be applied if ANY is selected in the dialog. Also
	'Chip external' access cycles are not valid in this mode. The dialog
	will validate the settings where possible.

        The dialog for a 'Data Access' and 'Data Value' breakpoint allows
	selection of 'Advanced' button. This will modify the breakpoint type
	to a 'Advanced' breakpoint. An 'Advanced' breakpoint can be directly
	specified in the Breakpoint window, or it can be optionally defined
	from an existing 'Data Access' or 'Data Value' breakpoint using the
	'Advanced' button. The 'Advanced' breakpoint allows a user familiar
	with the development target or debug hardware more detailed options.

	For example on the 32X and Saturn systems, the 'User Break Controller'
	(UBC) on an SH2, allows the definition of a breakpoint (possibly a
	mixture of Execution, Data Access/Value etc.).  The advanced dialog is
	more complex as it offers programming of all major UBC features
	directly.

	Creating or configuring a breakpoint of the above types from a
	Watch, Memory or Breakpoint Window now invokes a specific breakpoint
	dialog prior to the Generic Configuration dialog (which is updated
	accordingly with any values set for a breakpoint which are generic,
	such as address). An 'Execution' breakpoint specified in the
	Breakpoint Window or a Code style window only uses the Generic dialog.
	The user would need to define an 'Advanced' breakpoint from the
	breakpoint window from scratch, to define an enhanced execution style
	breakpoint.


	The basic settings for a breakpoint will be saved in the .INI file,
	such as address and data/mask values and access width. This is enough
	information to restore a Data Access/Value breakpoint. An 'Advanced'
	breakpoint will be restored with the same entries, however, as no
	assumptions can be made about an Advanced breakpoint, it will
        be ineffective until manual re-configuration has been made (as we
	don't specify various settings such as CPU cycle, access cycle etc.
	which effectively disable the breakpoint when not specified).


	Important
	=========
        The UBC functionality on the SH2 , specified that for a
	access breakpoint ('Data Access' and 'Data Value' breakpoints
	described above), the PC will be *near* the instruction that triggered
	the breakpoint. It cannot be accurately determined. The Debugger
	can still, however, identify the User's Breakpoint definition and
	subsequently apply the Generic Breakpoint conditions, such as
	Halt(don't), print expression to LOG window, conditional expression
        evaluation and count/trigger counts etc. as for execution breakpoints
	in the past.

        The Hitachi Hardware documentation for the SH2 has a useful Section 6
	about the UBC and how to program it. It will be of help when setting
        'Advanced' breakpoints on a 32X or Saturn system.

3.3.1.c Floating Point Support Added to Symbol Browsing
        -----------------------------------------------
        The Watch and Variables windows can now display the
        values of floating point data, float and double types. Note that such
        support has not been added to the expression evaluator.

        Note. This feature was first introduced in version 2.10z, 20
        November (Experimental Release).




3.3.2   What's Changed
        ==============

3.3.2.a User-Defined Breakpoints now Saved in session Files by Default
        --------------------------------------------------------------
	Breakpoints defined in the Breakpoint Window are now saved in
        Session files by default. Previously they would not be saved unless
        project information, such as a COFF, was loaded. Breakpoints defined in
        a Code style window (using F5 or Ctrl+F5 etc.) were saved correctly
        regardless of additional project info.

	This is fixed in that any user defined breakpoint is treated as
        project info. and will be saved unless over-ridden in the Session Save
        dialog.

3.3.2.b The Memory Window
        -----------------
	- The Memory window origin is now aligned using the display format. A
	Word/Long format display will mean that the origin address must be
        Word/Long aligned; any 'Goto Address' requests will be adjusted
	accordingly. This adjustment has been enforced to ensure correct
	breakpoint definition behaviour.

	To support the display of breakpoints, the following
	changes have been made:

	- The cursor highlight now has a grey background; it still identifies
	upper and lower nibbles of a byte.

	- The bytes to which a memory breakpoint has been applied are now
        displayed in yellow on a red background. The background colour changes
        to black when the breakpoint is disabled.

	- When memory is edited using the expression dialog, the bytes
	being edited are now displayed as yellow on green.


3.3.2.c Left Margin in Watch & Code windows reduced to 4 characters
        -----------------------------------------------------------
	The left-hand margin region in Code and Watch windows, which can be
	clicked on to toggle a breakpoint, has been reduced in width from 8
        characters to 4. The left-hand margin region is new to the Watch
        Window.


3.3.2.d Memory Window Display Changes - MWT
	-----------------------------------
        Memory window origin is now aligned using the display format. A
	Word/Long format display will mean the origin address must be
        Word/Long aligned, and any 'Goto Address' requests will be adjusted
	accordingly. This has been enforced to ensure proper breakpoint
	definition behaviour.

        To support the display of breakpoints (see 3.2.3.b below), the
        following changes have been made:

        - The cursor highlight now has a grey background; it still identifies
          upper and lower nibbles of a byte.
        - The bytes to which a memory breakpoint has been applied are now
          displayed as yellow on red background. The background is now black
          for a disabled breakpoint.
        - When memory is edited using the expression dialog, the bytes
          being edited are now displayed as yellow on green.


3.3.3   What's Fixed
        ============

3.3.3.a Watch Window Refreshing
	-----------------------
	Resizing the Watch window would cause it to collapse all structures
	being browsed. This is no longer the case.

3.3.3.b Breakpoint Dialogs and Mechanisms Fixed - MWT
	---------------------------------------------
	The 'Address' fields may now be modified on the Generic and Watch
	specific breakpoint configuration dialogs.

	When an invalid conditional expression is entered into the Generic
	Breakpoint dialog, this is now trapped at entry time, and when the
	breakpoint is evaluated at execution time.  A warning specifying the
	error in condition and breakpoint address is displayed at execution
	time.

	The trigger count is now saved as a LONG WORD instead of a WORD in the
	session .INI file. The trigger count is now used correctly the first
        time the condition count flag is checked in the dialog.


3.3.3.c Default Window Settings Now Saved Correctly
        --------------------------------------------
        Previously, the Save As Default option on the Display menu did not
        save the default window settings. This has now been fixed.


3.3.3.d Expression Dialog now accepts filenames from the displayed list
        ---------------------------------------------------------------
        Previously, selecting a file from the list of file names in the
        Expression Dialog did nothing. The file name is now placed in the
        expression field with a colon appended.


3.3.3.e File Server Buffer-Overflows Fixed
	----------------------------------
        Previously, the use of printf("\rHello") caused the debugger to
        crash. This was due to a buffer overflowing trying to
        print a 61K length string. This has now been fixed.

        Note. This fix was first introduced in version 2.10z, 20 November
        (Experimental Release).


3.3.3.f Saturn Monitor Failure on Downloading a COFF
        ---------------------------------------------
        Previously, downloading a SNASM COFF that sets the
        stack pointer and initialises the stack could cause the stack to be
        overwritten, causing the monitor to crash. This has now been fixed.

        Note. This fix was first introduced in version 2.10z, 20
        November (Experimental Release).


3.3.3.g Incorrect Data Displayed when Browsing Arrays in Compound Types
        ---------------------------------------------------------------
        Previously, the wrong data was displayed when browsing arrays
        contained within aggregate data types. This situation happened occurred
        when the definition line of an expanded array was scrolled out
        of view. This has now been fixed.

        Note. This fix was first introduced in version 2.10z, 20
        November (Experimental Release).


3.3.3.h Missing Type Information for Variables in the Watch Window
        -----------------------------------------------------------
        Previously, variables were sometimes displayed without type
        information in the Watch window. This has now been fixed.

        Note. This fix was first introduced in version 2.10z, 20
        November (Experimental Release).






3.4     The Debugger (V2.10c, 30 October 1995)
        ======================================
        IMPORTANT
        This Debugger release MUST be used in conjunction with the new Saturn
        monitor, dated 24/10/95. The date is shown on  the VDU's startup
        display when monitors are loaded.


3.4.1   What's New
        ==========

3.4.1.a Hardware Breakpoint Functionality Enabled - MWT
        -----------------------------------------------
        All defined breakpoints will be software breakpoints by default,
	unless ROM is encountered. The UBC controller for Saturn and 32X
	systems will now automatically be used for breakpoints in physical ROM
        memory (non-RAM emulated ROM).  This facilitates tracing and breaks
	etc. within ROM routines.

	User defined breakpoints may be configured as hardware by selecting the
	'H/W' flag and the 'Configure' dialog button in the Breakpoint Config
	dialog.

        The SH2 SH2's UBC has 2 channels, A and B. Channel B is more
	sophisticated and allows breakpoints on data bus conditions. Both
        channels on both SH2s. are utilized. The debugger will automatically
	allocate channel A/B resources as a definition requires them, whether
        user defined or defined by the Debugger during tracing.

        ***************************** IMPORTANT *****************************
        Modification of the interrupt masks in the SR register of the SH2 to
	level 15 will disable the UBC interrupt.  The Debugger will
	detect level 15 and will warn if level 15 currently set and
	disables definition. A trace may be aborted if neither software or
	hardware breakpoints are available and valid.

	No support can be provided for UBC complex data breakpoints and UBC
	execution breakpoints if the interrupt level is set to 15 during code
	execution between breakpoints.

	The Debugger will detect the disabling of internal interrupts when
        tracing LD[C|S] and ST[C|S] instructions using UBC breakpoints, which
	also affect the UBC functionality as above. These may appear as normal
	instructions, or as the SLOT of a SLOTTED instruction such as a JSR.

	The Debugger also prevent user toggled UBC breakpoints on instructions
	immediately after the LD[C|S] and ST[C|S] instructions.

        See "5.2 Support For the SH2's UBC (User Break Controller) for more
        details.


3.4.1.b New Saturn Monitor Release for Saturn - CROSS/SOA
	-------------------------------------------------
	The Saturn monitor was using a portion of the Sound memory for
	its own implementation. This was causing problems when a Sound Driver
	was loaded and overwrote the Monitor Data. This new Monitor no longer
	makes any demand for Sound memory.

	This problem was reported by the apparent inability of the Debugger
	to remove its defined software breakpoints within this Sound Driver
	code due to a link error caused by the loss of the monitor in this
	process because of memory conflicts.

        Note that the Saturn Monitor should display a Build date 24 October,
        1995 when the Debugger reloads the Monitor.


3.4.1.c Source File Directory Path - TDH
        --------------------------------
	A facility to create and maintain a list of directory names has been
	added.	These will be used to search for source files that cannot be
	located using information held in the COFF.  Lists are saved in the
	session file to allow different paths for each project.  The path list
	is modified via a dialog accessed from the "Source Directories" item
        on the Main Window "Session" menu or with the Ctrl+K accelerator.


3.4.2   What's Changed
        ==============

3.4.2.a Improved Heap overflow checking in libCross.a File
        --------------------------------------------------
	A new version of libcross.a is supplied with improved checking for
        heap overflow. A heap overflow now results in a warning printed
        to the log window and the calling function stopping with an abort.


3.4.2.b Special Trace Handling of SH2 TRAPA #32 Instruction - MWT/GNE
	-------------------------------------------------------------
	All TRAPA instructions may be traced akin to a JSR instruction, e.g.
	a F7 (single-step) will step into the trap code whereas F8 (step-over)
	will execute the trap code and stop on the next instr.

	A TRAPA #32 (Reserved for Debugger internal Usage) instruction,
	however, will cause F7 (single-step), Shift+F7 (forced-step) *and* F8
	(step-over) keys to step-over the trap #32 code. The Debugger uses
	TRAPA #32 instructions to support the FILESERVER library communications
	and other support mechanisms. It is not possible to trace into a
        trap #32 routine.


3.4.3   What's Fixed
        ============

3.4.3.a printf() in LibCross.a - GNE
	-----------------------------------------------
	When printf() commands went to the log window e.g.<stdout>,
	information was sometimes lost.  This was due to the wrong
	display command used in sending the data to be displayed. This
        has been fixed.

3.4.3.b Crash caused by following a void pointer fixed - TDH
	----------------------------------------------------
	A void pointer is now, correctly, only treated as data and so cannot
	be followed.


3.4.3.c Incorrect Processing of C++ Destructors
        ----------------------------------------
        Previously, destructors were not always processed correctly, e.g.
        '__$tmp_0.663'. This has now been fixed.


3.4.3.d "fake" Filename Removed from the File List
        ------------------------------------------
        Previously, the filename "fake" was visible in the list of files
        available from the Expression dialog. This has now been fixed happens.


3.4.3.e CapsLock on stops Key Presses Working
        -------------------------------------
        Having CapsLock on stopped some key presses working in the Source,
        Register, Mixed and Disassembly windows.  This has now been fixed.


3.4.3.f Structure Browse of Pointer Arrays
        ----------------------------------
        Previously, expanding arrays of pointers to one- or two-byte sized
        integral data types resulted in incorrect array indices and pointer
        values being displayed. This has now been fixed.


3.4.3.g Session Restore Window Co-ordinates - MWT
	-----------------------------------------------
	The window co-ordinates (especially initial Main Window) would
	be restrained to 80 x 25 screen dimensions (the default), due to the
        video subsystem info. not being initialized prior to the session
	file's co-ordinates being validated.


3.4.3.h Duplicate Symbols and the Local Variables Window
        ------------------------------------------------
        Previously, duplicate symbols in the COFF symbol table would sometimes
        cause the debugger to crash if the Local Variables Window was open.
        This has now been fixed.

===============================================================================
                                                        4.0 GENERAL INFORMATION
-------------------------------------------------------------------------------

4.1     The Configuration File (SNBUG.CFG)
        ==================================

        Always use the latest configuration file (SNBUG.CFG) as supplied on the
        SNASM2 distribution disks. The SNBUG.CFG file may contain new Memory
        Ranges and Default debugger settings that are referenced only when a
        user-defined Session File is not present.

        It is recommended that first time you use the debugger you should not
        use a Session File (.INI) (use the "-i" command-line switch to ignore
        any existing Session files). The debugger will then read SNBUG.CFG
        for the default Session settings. This first Session should then be
        saved in a new .INI file. To restore previous session information,
        either re-create the session and save the information to your new .INI
        file or copy the required settings from an existing .INI file.

4.2     Support for GNU C++ Function Name Decoding
        ==========================================

4.2.1   Introduction
        ============
        To provide the user of GNU C++ with the ability to enter qualified
        function names as expressions to the debugger, support for name
        decoding (sometimes called demangling) has been provided.  The decoder
        provides support for class functions and overloaded operators.
        Background information The decoded name presents the user with a
        readable representation of C++ function name (see reference 1 for
        description of function name encoding and reference 2 for the GNU
        exceptions to the rules defined in 1.). Below is an example of several
        encoded/decoded names.

        GNU Encoded name                   Decoded name
        ----------------                   ------------------------
        ___3foo                            foo::foo()
        ___3foofPCi                        foo::foo(const int*)
        __$_3foo                           foo::~foo()
        ___vc__3fooi                       foo::operator [](int)
        _display__C3fooi                   foo::display(int ) const


4.2.2   Usage information
        =================
        Within a valid expression field of the debuggers e.g. the 'Origin>Goto'
        or 'Utils>Expression Calculator' the user can enter the C++ qualified
        function name. This is then used to locate the corresponding symbol
        within the target program. Global symbols (i.e. static members) within
        a class can be browsed using a fully qualified name in the watch window
        (e.g. in example 1 below, foo::static_var, can be used to locate its
        value).

4.2.3   Name syntax
        ===========
        You must enter the full qualified name in order to resolve the symbol
        reference, e.g.  In example 1 below the user should enter

        foo::operator !

        to access the address of the overloaded operator !, within the class
        foo. The standard syntax would be;

         [class_name::] symbol_name

        where:
        class_name       you can have a list of class names separated by '::'
                         to form the qualified name

        symbol_name     the symbol name is either the name of a function or
                        operator.

        Note:  the insertion of spaces between various tokens within a symbol
        is irrelevant, e.g. the two symbol expressions below will be considered
        identical.

          foo    ::   operator    !
          foo::operator!

        The reason for the qualification can be shown from example 1, where it
        would be used to distinguish between the global and class foo overload
        of the new operator.  Below is a list of valid expression qualifier
        produced from the class definition in example 1;

        operator new                 // Global new

        foo::operator new            // Class foo overload of operator new

        foo::static_var              // Static variable

        foo::~foo                    // Class destructor

        foo::operator int            // Conversion operator from integer

        foo::operator foo2           // Conversion from another class type foo2

4.2.4   Examples
        ========

4.2.4.a Example 1. (section of dummy code for a class definition)
        ---------------------------------------------------------

void *operator new (size_t size)		// global overloaded operator
{
	return ::new char [size];
}

class foo					// dummy class foo
{
		// internal variables
		int foo_a, foo_b;

	public:
		// static variable
		static int static_var;

		// constructors & destructor
		foo() { foo_a = 0; foo_b = 0; };
		foo(int a, int b = 0) {foo_a = a; foo_b = b};
		virtual ~foo() {};

		// class functions
		virtual void display();
		void *operator new(size_t size) {return ::new char [size];};
		foo &operator !(foo &a);

		// conversion operators
		operator int();
		operator foo2();
};

int foo::static_var;				// storage space for static


4.2.4.b Example 2
        ----------
        In using the goto option on the debuggers Origin menu, you enter in an
        incorrect symbol name (i.e. 'foo::operator |' instead of
        'foo::operator !'), the expresssion parser will report the error at the
        '::' token rather than the '|'.  The reason for this is that it cannot
        evaluate which of the C++ tokens are incorrect, so it reports the
        first it is unsure about.


4.2.4.c Example 3
        ---------
        There is no change in the use of the watch window to inspect class
        variables, they are treated as structures.


4.2.5   Limitations and Consideration
        =============================

4.2.5.a Overloaded Functions
        --------------------

        Overloaded functions (i.e. the two constructors in example 1) cannot
        be distinguished apart using only the qualified name (i.e. foo::foo).


4.2.5.b Global Constructors
        -------------------
        Global constructors / destructors are created if the user defines a
        class variable with file scope.  This normally introduces two special
	labels;
	   __GLOBAL_$I$<some string FROM a random class function NAME>
	   __GLOBAL_$D$<some string FROM a random class function NAME>
        These functions are not decoded as no information is currently
        available on how they are generated.


4.2.5.c Mixed Window
        ------------
        In the mixed window mode if the user request a 'Origin>goto' to a
        compiler generated default constructors or destructors (i.e. those
        produced by the compiler when no user defined option exists) erroneous
        line references can be encountered in the source window.  The reason
        for this is that no source code exists, so it goes to the line number
        that was assigned just before the compiler created the code.  This is
        normally (but not always) at the end of the class definition.

4.2.5.d Indirection
        -----------
        With C++ when referencing both sub-structures & sub-classes within the
        browse feature of a watch window, the user is required to follow two
        levels of indirection and not just the normal one provided by the C
        compiler.  We are not sure what is causing this, though it is
        suspected that it is the way that both structures & class definition
        include a 'type def' by default in C++ but not in C.

4.2.5.e Pointers to an Unknown Class
        ----------------------------
        Within classes that where derived virtually from parent classes or
        contain virtual functions there exist certain variables (seen by the
        Watch window as pointers to an unknown class), e.g.  **$vf, **$vb.
        You can follow these references, but the format of what is at the
        other end is unknown.


4.2.6   References
        ==========

4.2.6.a The Annotated C++ Reference Manual
        ----------------------------------
	Margaret A. Ellis & Bjarne Stroustrup,
	Addison-Wesley Publishing Company,
	ISBN 0-201-51459-1.

        Page 123-127. describes the basic process of function name
        encoding/decoding.

4.2.6.b GNU C++ Renovation Project (phase 1)
        ------------------------------------
        Information, Free Software Foundation Inc,
        Chapter 4, Name encoding in GNU C++.


4.3     Fileserver Library (LIBCROSS.A)
        ===============================

4.3.1   Introduction
        ============
        This section describes a new version of the GNU C library 'libc.a'
        designed for use with the SH2 on either a Saturn in conjunction with
        the Cross Products SNASM debugger version 2.10b (or later). The new
        library allows you to access the PC as a file server for standard I/O
        commands (e.g. printf(), fopen(), fclose(), fseek(), etc.).

        Default channels:
	stdin 	not supported. (message displayed, no action performed)
	stdout	Sent to log window (prefixed <stdout> and coloured green)
	stderr	Sent to log window (prefixed <stderr> and coloured cyan).
	      	stderr is a byte stream output (i.e. each character appears
	      	on a separate line).

4.3.2   Supported Commands
        ==================

        The following stdio commands are supported;
	fclose(), feof(), fflush(), fgetc(), fgetpos(), fgets(),
	fopen(), fdopen(), fputc(), fputs(), fread(), fseek(),
	fsetpos(), ftell(), fwrite(), getc(), perror(), putc(),
	putchar(), puts(), rewind(), printf(), fprintf(), fscanf().

        The remaining commands are not recommended (they might work but have
        not been tested exhaustively):
	clearerr(), ferror(), fiprintf(), getchar(), gets(), iprintf(),
	mktemp(), remove(), rename(), setbuf(), setvbuf(), siprintf(),
	scanf(), tmpfile(), tmpnam(), vprintf().


4.3.3   Special comments on certain commands
        ====================================

4.3.3.a
        When opening a file on the PC hard disk (i.e. open(), fopen(), etc.),
        you can specify either binary and text (the default) mode of
        operation, e.g. fopen(filename, "r");   // defaults to text mode
        fopen(filename, "rb");  // binary mode

4.3.3b
        Any files that are open when the COFF is reloaded are closed
        automatically by the debugger.
4.3.3.c
        No information can be accessed from standard in <stdin>. This is why
	several functions (scanf(), getchar(), etc) are noted as not usable.

4.3.3.d
       Note: PCs have a limited number of files that can be opened at any time.


4.3.4   Compiler Usage
        ==============

        The new library "libCross.a" should be placed in the same directory as
        all other GNU libraries.  Example Compilation line for stand-alone
        modules:

	gcc -m2   -g   -nostdlib   \<gnu_path>\lib\crt0.o <user files>
	-Xlinker -T   -Xlinker <link file>   -lCross   -lgcc

        where;
	-m2		indicates SH2.
	-g		- debugging information.
	-nostdlib	- don’t link any standard libraries.
	crt0.o		- the start-up file.
	<user files>	- user target files to compile.
	-Xlinker	- indicates next command to pass to linker
        -T              -
	<link file>	- user defined linker information.
	-lCross		- new clib.
	-lgcc		- gcc library.


4.3.5   Comments
        ========
        The file server librabry operates in either normal or minimum hit mode
        i.e. CTRL+U, this is an advantage over using TCL macros, since they
        operate only in normal mode & are then dependent on the processor
        refresh rate. In both modes the server operates independently of the
        processors' refresh rate.

        The library is not re-entrant, though it is semaphored, i.e. both SH2’s
        should not attempt to access the C library as both processor currently
        share the same re-entrant structure. If you define a different
re-entrant structure for the sub-processor when it is started, the library
        will still work.


4.3.6   Reserved variables
        ==================
        The debugger uses two variables (structures) in order to define the
        communications protocol. You must not substitute or define variables of
        the same name.  The two variables are;
        __SNASM_Clib_server_buffer
	__SNASM_Clib_server

        A single internal variable (__SNASM_Clib_trap) is used as a flag to
        control the functionality of the remaining system calls (i.e. _exit(),
        _kill(), _chmod(), _chown(), _utime(), _fork(), _wait(),_execve(),
        _execv(), _pipe()) in the C library.

        The library operates in two ways;

	default :- calls a series of prints to warn the user of access to these
	functions & performs a default safe action for each call.

	when flag set :- the routines call "trapa #3" with a series of
	parameters as defined in the original gnu stubs.  The flag can be set
	by calling the additional function ‘set_trap3_calls_active()’.


4.3.7   The Error Flag
        ==============
        "errno" is converted from the PC representation to the ones defined by
        GNU within the library.  If an equivalent flag does not exist e.g.
        (EZERO and ENAMETOOLONG) then the default value of '0' is returned.

4.3.8   Heap Space
        ==========
        You must set-up a heap, this can be done in two ways;

	i.	use the 'end' variable set-up by the gnu compiler as part of
		the linker file.
	ii.	call the additional function
			initialise_heap(int start, int size)

		before any library calls in your program.

4.3.9   Debugger Command-line option
        ============================
        A new command line option '-c' as been added to the debugger command-
        line.  This option causes then debugger to display additional
        information in the log window as each file server command is
        implemented.


4.3.10  GNU Anomalies
        =============
        If problems arise (In testing this only occurred infrequently and
        then mostly on unsigned shorts) in the passing of parameters to the I/O
        routines e.g. code like this

	unsigned short a = 0x0123, b = 0x4567;
	unsigned short c = 0x89ab, d = 0xcdef

	printf("a=0x%x, b=0x%x, c=0x%x, d=0x%x\n",(unsigned int)a,
	                                          (unsigned int)b,
	                                          (unsigned int)c,
	                                          (unsigned int)d);
        produces, output like

	a=0xFFFF0123, b=0xFFFF4567, c=0xFFFF89ab, d=0xFFFFcdef

        This results from the user compiling the code with the gnu flags -O2,
        this tends to occasionally optimize out some of the type cast.





===============================================================================
                                                5.0 TARGET SPECIFIC INFORMATION
-------------------------------------------------------------------------------

5.1     CartDev Power Cycling Issues [32X and Saturn only]
        ==================================================

        ALWAYS power the CartDev before powering the Saturn (or Programming
        Box). Powering the Saturn (or Programming Box) before the CartDev may
        damage the development target.

        DO NOT power off the CartDev during a debugging session. This may cause
        the following problems:

        - You may lose contact at the ASPI device driver level, giving the
          appearance that the debugger has hung (it hasn't).

        - The Adaptec SCSI controller may lock-up with the effect of losing
          access to all devices on the SCSI bus (including your HD if present
          on the bus). If the SCSI controller has locked-up then its the red
          LED and/or your development PC's HD access light will be permanently
          lit. The development PC must then be power-cycled to reset the SCSI
          controller.

        If you need to power-cycle the CartDev, for example when the SCSI link
        cannot be re-established, do so at a DOS prompt when no device is
        attempting SCSI communication.

        DO NOT connect/disconnect the SCSI cable to/from the CartDev when
        power is on as this may damage the unit.

        ALWAYS power off the Saturn (or Programming Box) before powering off
        the CartDev.


5.2     Support For the SH2's UBC (User Break Controller)
        =================================================

5.2.1   Channel Availability
        ===================
        The UBC channels are ALWAYS available for Tracing, Run to cursor and
        Run to Address requests, and will be used when necessary.

        The Debugger will automatically use hardware (UBC) breakpoints when a
        software breakpoint definition fails. This behaviour enables the
        debugger to support ROM tracing (important on Saturn).


5.2.2   Channel Management
        ==================
        The SH2's UBC has 2 channels, A and B. Channel B is more sophisticated
        and allows breakpoints on data bus conditions. The debugger uses both
        channels on both SH2s. Breakpoints may be configured through the
        Configure UBC dialog. The debugger automatically allocates channel A/B
        resources as required by a breakpoint definition.




===============================================================================
                                                    6.0 LAST MINUTE INFORMATION
-------------------------------------------------------------------------------

6.1     Last Minute Information
        =======================
        There is no last minute information for this release.





===============================================================================
Copyright 1995-96, Cross Products Limited.
SNASM2 is a Trademark of Cross Products Limited. All other Trademarks and
Registered Trademarks acknowledged.
----------------------------------------------------------------End of Document