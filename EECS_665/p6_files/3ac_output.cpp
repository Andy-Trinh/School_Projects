#include "ast.hpp"

namespace a_lang{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);

	Procedure * proc = prog->makeProc(sym->getName());

	for (auto formal : *myFormals) {
		formal->to3AC(proc);
	}
	
	size_t i = 1;
	for (SymOpd * opd : proc->getFormals()) {
		proc->addQuad(new GetArgQuad(i, opd));
		i++;
	}

	for (auto body : *myBody) {
		body->to3AC(proc);
	}
}

void FnDeclNode::to3AC(Procedure * proc){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A function declaration
	// never occurs within another function)
	throw new InternalError("FnDecl at a local scope");
}

void FormalDeclNode::to3AC(IRProgram * prog){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A formal never
	// occurs at global scope)
	throw new InternalError("Formal at a global scope");
}

void FormalDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherFormal(sym);
}

Opd * IntLitNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd(std::to_string(myNum), 8);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * TrueNode::flatten(Procedure * proc){
	Opd * res = new LitOpd("1", 8);
	return res;
}

Opd * FalseNode::flatten(Procedure * proc){
	Opd * res = new LitOpd("0", 8);
	return res;
}

Opd * EhNode::flatten(Procedure * proc){
	const DataType * dt = BasicType::produce(BaseType::BOOL);
	SemSymbol * semSym = new VarSymbol("randBool", dt);

	proc->addQuad(new CallQuad(semSym));
	Opd * retOpd = proc->makeTmp(8);
	proc->addQuad(new GetRetQuad(retOpd));
	return retOpd;
	
}

Opd * CallExpNode::flatten(Procedure * proc){
	size_t i = 1;
	for (auto arg : *myArgs){
		Opd * argOpd = arg->flatten(proc);
		proc->addQuad(new SetArgQuad(i, argOpd));
		i++;
	}

	Quad * calleeQuad = new CallQuad(myCallee->getSymbol());
	proc->addQuad(calleeQuad);

	const FnType * calleeType = myCallee->getSymbol()->getDataType()->asFn();

	if (calleeType->getReturnType()->isVoid()){
		return nullptr;
	} else { 
		Opd * retOpd = proc->makeTmp(8);
		Quad * getRet = new GetRetQuad(retOpd);
		proc->addQuad(getRet);
		return retOpd; 
	}
}

Opd * NegNode::flatten(Procedure * proc){
	Opd * expOpd = myExp->flatten(proc);

	Opd * opRes = proc->makeTmp(8);

	UnaryOpQuad * quad = new UnaryOpQuad(opRes, NEG64, expOpd);
	proc->addQuad(quad);
	return opRes;
}

Opd * NotNode::flatten(Procedure * proc){
	Opd * expOpd = myExp->flatten(proc);

	Opd * opRes = proc->makeTmp(8);

	UnaryOpQuad * quad = new UnaryOpQuad(opRes, NOT64, expOpd);
	proc->addQuad(quad);
	return opRes;
}

Opd * PlusNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, ADD64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * MinusNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, SUB64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * TimesNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, MULT64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * DivideNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, DIV64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * AndNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, AND64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * OrNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, OR64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * EqualsNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, EQ64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, NEQ64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * LessNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, LT64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * GreaterNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, GT64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * LessEqNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, LTE64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	Opd * opd1 = myExp1->flatten(proc);
	Opd * opd2 = myExp2->flatten(proc);

	Opd * opRes = proc->makeTmp(8);
	BinOpQuad * quad = new BinOpQuad(opRes, GTE64, opd1, opd2);
	proc->addQuad(quad);
	return opRes;
}

void AssignStmtNode::to3AC(Procedure * proc){
	Opd * lhs = myDst->flatten(proc);
	Opd * rhs = mySrc->flatten(proc);

	Quad * quad = new AssignQuad(lhs, rhs);
	proc->addQuad(quad);
}

void MaybeStmtNode::to3AC(Procedure * proc){
	const DataType * dt = BasicType::produce(BaseType::BOOL);
	SemSymbol * semSym = new VarSymbol("randBool", dt);

	proc->addQuad(new CallQuad(semSym));
	Opd * retOpd = proc->makeTmp(8);
	proc->addQuad(new GetRetQuad(retOpd));

	Label * lbl1 = proc->makeLabel();
	Label * lbl2 = proc->makeLabel();

	proc->addQuad(new IfzQuad(retOpd, lbl1));
	proc->addQuad(new GotoQuad(lbl2));
	
	Quad * meansQuad = new NopQuad();
	meansQuad->addLabel(lbl1);
	proc->addQuad(meansQuad);

	Quad * otherwiseQuad = new NopQuad();
	otherwiseQuad->addLabel(lbl2);
	proc->addQuad(otherwiseQuad);
	
	
}

void PostIncStmtNode::to3AC(Procedure * proc){
	Opd * locOpd = myLoc->flatten(proc);
	Opd * litOpd = new LitOpd("1", 8);
	proc->addQuad(new BinOpQuad(locOpd, ADD64, locOpd, litOpd));

}

void PostDecStmtNode::to3AC(Procedure * proc){
	Opd * locOpd = myLoc->flatten(proc);
	Opd * litOpd = new LitOpd("1", 8);
	proc->addQuad(new BinOpQuad(locOpd, SUB64, locOpd, litOpd));
}

void ToConsoleStmtNode::to3AC(Procedure * proc){
	Opd * srcOpd = mySrc->flatten(proc);
	const DataType * dt = BasicType::produce(BaseType::INT);
	proc->addQuad(new WriteQuad(srcOpd, dt));
}

void FromConsoleStmtNode::to3AC(Procedure * proc){
	Opd * dstOpd = myDst->flatten(proc);
	const DataType * dt = BasicType::produce(BaseType::INT);
	proc->addQuad(new ReadQuad(dstOpd, dt));
}

void IfStmtNode::to3AC(Procedure * proc){
	Label * exitLbl = proc->makeLabel();
	Opd * cndOpd = myCond->flatten(proc);

	proc->addQuad(new IfzQuad(cndOpd, exitLbl));

	for (auto stmt : *myBody) {
		stmt->to3AC(proc);
	}

	Quad * exitQuad = new NopQuad();
	exitQuad->addLabel(exitLbl);
	proc->addQuad(exitQuad);
	
}

void IfElseStmtNode::to3AC(Procedure * proc){
	Label * elseLbl = proc->makeLabel();
	Label * exitLbl = proc->makeLabel();
	Opd * cndOpd = myCond->flatten(proc);
	
	proc->addQuad(new IfzQuad(cndOpd, elseLbl));

	for (auto stmt : *myBodyTrue) {
		stmt->to3AC(proc);
	}

	proc->addQuad(new GotoQuad(exitLbl));

	Quad * exitElseQuad = new NopQuad();
	exitElseQuad->addLabel(elseLbl);
	proc->addQuad(exitElseQuad);

	for (auto stmt : *myBodyFalse) {
		stmt->to3AC(proc);
	}

	Quad * exitQuad = new NopQuad();
	exitQuad->addLabel(exitLbl);
	proc->addQuad(exitQuad);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Label * loopLbl = proc->makeLabel();
	Label * exitLoopLbl = proc->makeLabel();

	Quad * startQuad = new NopQuad();
	startQuad->addLabel(loopLbl);
	proc->addQuad(startQuad);

	Opd * cndOpd = myCond->flatten(proc);

	proc->addQuad(new IfzQuad(cndOpd, exitLoopLbl));

	for (auto stmt : *myBody) {
		stmt->to3AC(proc);
	}

	proc->addQuad(new GotoQuad(loopLbl));

	Quad * exitQuad = new NopQuad();
	exitQuad->addLabel(exitLoopLbl);
	proc->addQuad(exitQuad);
}

void CallStmtNode::to3AC(Procedure * proc){
	myCallExp->flatten(proc);
}

void ReturnStmtNode::to3AC(Procedure * proc){
	
	if (myExp != nullptr) {
		Opd * retOpd = myExp->flatten(proc);
		proc->addQuad(new SetRetQuad(retOpd));
	}

	proc->addQuad(new GotoQuad(proc->getLeaveLabel()));
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd * IDNode::flatten(Procedure * proc){
	SemSymbol * sym = this->getSymbol();
	Opd * res = proc->getSymOpd(sym);
	return res;
}

}
