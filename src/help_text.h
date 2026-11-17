/*
	Chess for Casio PRIZM calculators
	Copyright (C) 2013  Balázs Dura-Kovács

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

const int help_sorok_szama=126;
const char help_text[] = "       Chess v1.01\n\nPlay chess on your own\nCasio PRIZM calculator\nwith your friend. Chess\nwhich is one of the most \nfamous and popular games\nin the world is now ready\nto be played everywhere\non your calculator. Best\ngame that can be played\nunnoticed  during boring\nmaths or physics lessons!\n\n\n      *** Rules ***\n\nEverybody can play chess!\n\nSupported special moves:\n   Castling, En passant,\n   Promotion\nUnsupported special rules\n   Fifty-move rule,\n   Threefold repetition\n\n\n    *** Controls ***\n\nPress [F1] or [F6]\n   to display the help.\nPress [F4]\n   to start a new game.\nPress [F5] to\n   launch the setup menu.\n\n   Moving a piece: select\na piece using the arrow\nkeys. Press [EXE]. Select\nanother square where you\nwant to move. Press [EXE]\nagain. Movements are can-\ncelable using the [EXIT]\nkey. Instead of using the\narrow keys you can press\nkeys A-H and 1-8 to se-\nlect a column or a row.\n   Castling: move your\nking two squares along\nthe first rank.\n   Promotion: select a\nnew piece using the arrow\nkeys then press [EXE].\n\n\n      *** Setup ***\n\nThere are two methods of\nrotating the board and\nthe pieces:\n   Static: in this mode\nthe board and the pieces\naren't rotated during the\ngameplay. You can set the\nwhite and black side and\nyou can set the direc-\ntions of the white and\nblack pieces.\n   Dynamic: in this mode\nthe board and the pieces\nare rotated after every\nmovement. You can set the\nside of the current pla-\nyer (who is symbolized by\nwhite) and the direction\nof the pieces of the cur-\nrent and of the opponent\nplayer.\n\n\n      *** Files ***\n\nThis game saves two files\nto the main memory. You\ncan find these in\nMemory Manager/\n   Main Memory/\n      @CHESS\n\nThese files are:\nGAME: contains your cur-\n   rent gameplay. If you\n   delete it, you won't\n   be able to continue\n   your paused game.\nSETTINGS:\n   contains your settings\n   If you delete it, your\n   settings are restored\n   to default.\n\n\n       *** Bugs ***\n\nPlease note that this is\nthe first release of this\nchess program. The chess\nengine might contain bugs.\nPlease send bug reports\nto my e-mail address (be-\nlow) or post it at the\nforum thread at Chemetech\n( http://goo.gl/wQlzGb ).\n\n\n     *** Credits ***\n\nDeveloped by Balping\nbalping.official@gmail.com\n(C) balping 2013\n\nThis package is\ndownloadable at\ncemetech.net\n\n(Press [EXIT].)";
