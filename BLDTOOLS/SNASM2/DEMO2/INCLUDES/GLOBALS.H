;****************************************************************
;	Saturn Sample Program
;	Master sh2 global variable externs
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	xref	sincosbuf, transtmp
	xref	_drawcounter
	xref	_temppoints, _spritebuf, _vbrsave
		
	xref 	_zbuffer

	xref	_randval
	xref	_intmask
	xref	_VDP1busy, _drawflag, _clearflag
	xref	_vcounter
	xref	_pad0, _pad1
	xref	_numsprites
	xref	_lastsprite


	xref	_camerax, _cameray, _cameraz
	xref	_objectx, _objecty, _objectz
	xref	_pitch, _yaw, _bank
