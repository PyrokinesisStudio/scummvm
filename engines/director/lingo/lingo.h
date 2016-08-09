/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef DIRECTOR_LINGO_LINGO_H
#define DIRECTOR_LINGO_LINGO_H

#include "common/debug.h"
#include "common/hashmap.h"
#include "common/hash-str.h"
#include "audio/audiostream.h"
#include "common/str.h"
#include "engines/director/director.h"
#include "engines/director/score.h"
#include "director/lingo/lingo-gr.h"
#include "director/lingo/lingo-the.h"

namespace Director {

enum LEvent {
	kEventPrepareMovie,
	kEventStartMovie,
	kEventStopMovie,

	kEventNew,
	kEventBeginSprite,
	kEventEndSprite,

	kEventNone,
	kEventEnterFrame,
	kEventPrepareFrame,
	kEventIdle,
	kEventStepFrame,
	kEventExitFrame,

	kEventActivateWindow,
	kEventDeactivateWindow,
	kEventMoveWindow,
	kEventResizeWindow,
	kEventOpenWindow,
	kEventCloseWindow,

	kEventKeyUp,
	kEventKeyDown,
	kEventMouseUp,
	kEventMouseDown,
	kEventRightMouseUp,
	kEventRightMouseDown,
	kEventMouseEnter,
	kEventMouseLeave,
	kEventMouseUpOutSide,
	kEventMouseWithin,

	kEventStart
};

typedef void (*inst)(void);
#define	STOP (inst)0

typedef Common::Array<inst> ScriptData;
typedef Common::Array<double> FloatArray;

struct Symbol {	/* symbol table entry */
	char *name;
	int type;
	union {
		int		i;			/* VAR */
		double	f;			/* FLOAT */
		ScriptData	*defn;	/* FUNCTION, PROCEDURE */
		void (*func)();		/* OPCODE */
		void (*bltin)(int);	/* BUILTIN */
		Common::String	*s;	/* STRING */
		FloatArray *arr;	/* ARRAY, POINT, RECT */
	} u;
	int nargs;		/* number of arguments */
	int maxArgs;	/* maximal number of arguments, for builtins */
	bool parens;	/* whether parens required or not, for builitins */

	bool global;

	Symbol();
};

struct Datum {	/* interpreter stack type */
	int type;

	union {
		int	i;
		double f;
		Common::String *s;
		Symbol	*sym;
		FloatArray *arr;	/* ARRAY, POINT, RECT */
	} u;

	Datum() { u.sym = NULL; type = VOID; }

	double toFloat();
	int toInt();
	Common::String *toString();

	const char *type2str(bool isk = false);
};

struct Builtin {
	void (*func)(void);
	int nargs;

	Builtin(void (*func1)(void), int nargs1) : func(func1), nargs(nargs1) {}
};

typedef Common::HashMap<int32, ScriptData *> ScriptHash;
typedef Common::Array<Datum> StackData;
typedef Common::HashMap<Common::String, Symbol *, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> SymbolHash;
typedef Common::HashMap<Common::String, Builtin *, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> BuiltinHash;

typedef Common::HashMap<Common::String, TheEntity *, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> TheEntityHash;
typedef Common::HashMap<Common::String, TheEntityField *, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> TheEntityFieldHash;

struct CFrame {	/* proc/func call stack frame */
	Symbol	*sp;	/* symbol table entry */
	int		retpc;	/* where to resume after return */
	ScriptData	*retscript;	 /* which script to resume after return */
	SymbolHash *localvars;
};

class Lingo {
public:
	Lingo(DirectorEngine *vm);
	~Lingo();

	void addCode(const char *code, ScriptType type, uint16 id);
	void executeScript(ScriptType type, uint16 id);

	void processEvent(LEvent event, int entityId);

	void initBuiltIns();
	void initTheEntities();

	Common::String *toLowercaseMac(Common::String *s);

	void runTests();

private:
	const char *findNextDefinition(const char *s);

public:
	void execute(int pc);
	void pushContext();
	void popContext();
	Symbol *lookupVar(const char *name, bool create = true, bool putInGlobalList = false);
	void cleanLocalVars();
	void define(Common::String &s, int start, int nargs, Common::String *prefix = NULL);
	void processIf(int elselabel, int endlabel);

	int alignTypes(Datum &d1, Datum &d2);

	int code1(inst code) { _currentScript->push_back(code); return _currentScript->size() - 1; }
	int code2(inst code_1, inst code_2) { int o = code1(code_1); code1(code_2); return o; }
	int code3(inst code_1, inst code_2, inst code_3) { int o = code1(code_1); code1(code_2); code1(code_3); return o; }
	int codeString(const char *s);
	void codeLabel(int label);
	int codeConst(int val);

	int calcStringAlignment(const char *s) {
		return calcCodeAlignment(strlen(s) + 1);
	}
	int calcCodeAlignment(int l) {
		int instLen = sizeof(inst);
		return (l + instLen - 1) / instLen;
	}

	void codeArg(Common::String *s);
	void codeArgStore();
	int codeFunc(Common::String *s, int numpar);
	int codeFloat(double f);
	void codeFactory(Common::String &s);

	void pushVoid();

	static void c_xpop();
	static void c_printtop();

	static void c_add();
	static void c_sub();
	static void c_mul();
	static void c_div();
	static void c_negate();

	static void c_and();
	static void c_or();
	static void c_not();

	static void c_ampersand();
	static void c_concat();
	static void c_contains();
	static void c_starts();

	static void c_intersects();
	static void c_within();

	static void c_constpush();
	static void c_voidpush();
	static void c_fconstpush();
	static void c_stringpush();
	static void c_varpush();
	static void c_assign();
	bool verify(Symbol *s);
	static void c_eval();

	static void c_swap();

	static void c_theentitypush();
	static void c_theentityassign();

	static void c_repeatwhilecode();
	static void c_repeatwithcode();
	static void c_ifcode();
	static void c_eq();
	static void c_neq();
	static void c_gt();
	static void c_lt();
	static void c_ge();
	static void c_le();
	static void c_call();
	static void c_procret();

	static void c_mci();
	static void c_mciwait();
	static void c_goto();
	static void c_gotoloop();
	static void c_gotonext();
	static void c_gotoprevious();
	static void c_global();

	static void c_play();
	static void c_playdone();

	static void c_open();

	static void b_abs(int nargs);
	static void b_atan(int nargs);
	static void b_chars(int nargs);
	static void b_cos(int nargs);
	static void b_exp(int nargs);
	static void b_float(int nargs);
	static void b_integer(int nargs);
	static void b_length(int nargs);
	static void b_log(int nargs);
	static void b_pi(int nargs);
	static void b_power(int nargs);
	static void b_random(int nargs);
	static void b_sin(int nargs);
	static void b_sqrt(int nargs);
	static void b_string(int nargs);
	static void b_tan(int nargs);

	static void b_ilk(int nargs);
	static void b_alert(int nargs);
	static void b_cursor(int nargs);
	static void b_printFrom(int nargs);
	static void b_showGlobals(int nargs);
	static void b_showLocals(int nargs);

	static void b_editableText(int nargs);
	static void b_installMenu(int nargs);
	static void b_updateStage(int nargs);
	static void b_moveableSprite(int nargs);
	static void b_puppetPalette(int nargs);
	static void b_puppetSound(int nargs);
	static void b_puppetSprite(int nargs);
	static void b_puppetTempo(int nargs);
	static void b_puppetTransition(int nargs);
	static void b_spriteBox(int nargs);
	static void b_zoomBox(int nargs);

	static void b_continue(int nargs);
	static void b_dontPassEvent(int nargs);
	static void b_delay(int nargs);
	static void b_do(int nargs);
	static void b_nothing(int nargs);
	static void b_pause(int nargs);
	static void b_playAccel(int nargs);
	static void b_quit(int nargs);
	static void b_restart(int nargs);
	static void b_shutDown(int nargs);
	static void b_startTimer(int nargs);

	static void b_closeDA(int nargs);
	static void b_closeResFile(int nargs);
	static void b_closeXlib(int nargs);
	static void b_openDA(int nargs);
	static void b_openResFile(int nargs);
	static void b_openXlib(int nargs);
	static void b_showResFile(int nargs);
	static void b_showXlib(int nargs);

	static void b_point(int nargs);

	static void b_beep(int nargs);

	void func_mci(Common::String &s);
	void func_mciwait(Common::String &s);
	void func_goto(Datum &frame, Datum &movie);
	void func_gotoloop();
	void func_gotonext();
	void func_gotoprevious();

public:
	void setTheEntity(int entity, Datum &id, int field, Datum &d);
	void setTheSprite(Datum &id, int field, Datum &d);
	void setTheCast(Datum &id, int field, Datum &d);
	Datum getTheEntity(int entity, Datum &id, int field);
	Datum getTheSprite(Datum &id, int field);
	Datum getTheCast(Datum &id, int field);

public:
	ScriptData *_currentScript;
	ScriptType _currentScriptType;
	bool _returning;
	bool _indef;

	Common::Array<CFrame *> _callstack;
	Common::Array<Common::String *> _argstack;
	TheEntityHash _theEntities;
	TheEntityFieldHash _theEntityFields;
	Common::Array<int> _labelstack;

	SymbolHash _handlers;

	int _linenumber;
	int _colnumber;

	Common::String _floatPrecisionFormat;

	bool _hadError;

	bool _inFactory;
	Common::String _currentFactory;

private:
	int parse(const char *code);
	void push(Datum d);
	Datum pop(void);

	Common::HashMap<uint32, const char *> _eventHandlerTypes;
	Common::HashMap<Common::String, Audio::AudioStream *> _audioAliases;

	ScriptHash _scripts[kMaxScriptType + 1];

	SymbolHash _globalvars;
	SymbolHash *_localvars;

	int _pc;

	StackData _stack;

	DirectorEngine *_vm;

	int _floatPrecision;
};

extern Lingo *g_lingo;

} // End of namespace Director

#endif