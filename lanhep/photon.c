#include "lanhep.h"

int verb_charge = 0;

static Atom ph_f=0, ph_c=0;

int defined_em_charge(void)
	{
	return ph_f;
	}

Term ProcSetEM(Term t, Term ind)
	{
	if(!is_compound(t) || CompoundArity(t)!=2 ||
		!is_atom(CompoundArg1(t)) || !is_atom(CompoundArg2(t)))
			{
			ErrorInfo(226);
			puts("bad argumants in SetEM call.\n");
			return 0;
			}
	if(ph_f)
		{
		ErrorInfo(227);
		puts("multiple calls of SetEM.");
		}
	ph_f=CompoundArg1(t);
	ph_c=CompoundArg2(t);
	A_EE=ph_c;
	FreeAtomic(t);
	return 0;
	}
	
	
static void set_sc_ch(Term a2, Atom p1, Atom p2, int php)
	{
	List l;
	int num,den,cd;
	Integer mprt, mind;
	a2=CopyTerm(a2);
	alg2_common_n(a2);
	alg2_common_t(a2);
	alg2_red_cos(a2);
	alg2_red_orth(a2);
	if(php==1)
		mprt=NewInteger(2);
	else
		mprt=NewInteger(1);
	mind=ListFirst(CompoundArg1(CompoundArg2(ListNth(CompoundArg1(a2),php))));
	
	l=CompoundArgN(a2,5);
	if(is_empty_list(l))
		{
		if(verb_charge)
				{
				printf("Empty vertex ");
				WriteVertex(CompoundArg1(a2));
				printf(", assuming charges are zero.\n");
				}
		FreeAtomic(a2);
		return;
		}
	while(!is_empty_list(l))
		{
		Term m2;
		List cn,sp;
		m2=ListFirst(l);
		cn=CompoundArg2(m2);
		if(ListLength(cn)!=1)
			goto cnt;
		cn=ListFirst(cn);
		if(CompoundArg1(cn)!=ph_c || CompoundArg2(cn)!=NewInteger(1))
			goto cnt;
		sp=CompoundArgN(m2,3);
		if(ListLength(sp)==1 && CompoundName((cn=ListFirst(sp)))==A_MOMENT  && 
			CompoundArg1(cn)==mprt && ListFirst(CompoundArg2(cn))==mind)
			{
			num=IntegerValue(CompoundArg1(CompoundArg2(a2)));
			num*=IntegerValue(CompoundArg1(m2));
			den=IntegerValue(CompoundArg2(CompoundArg2(a2)));
			cd=gcf(num,den);
			num/=cd;
			den/=cd;
			SetAtomProperty(p1,A_EM_CHARGE,
				MakeCompound2(OPR_DIV,NewInteger(-num), NewInteger(den)));
			SetAtomProperty(p2,A_EM_CHARGE,
				MakeCompound2(OPR_DIV,NewInteger(num), NewInteger(den)));
			if(verb_charge)
				{
				printf("Charges in vertex ");
				WriteVertex(CompoundArg1(a2));
				printf(" : %d/%d and %d/%d\n",-num,den,num,den);
				}
			FreeAtomic(a2);
			return;
			}
	cnt:
		l=ListTail(l);
		}
	
	printf("Warning: can't determine electric charge from vertex ");
	WriteVertex(CompoundArg1(a2));
	puts(".");
	
	
	
	}
	
	
static void set_sp_ch(Term a2, Atom p1, Atom p2, int php)
	{
	List l;
	int num,den,cd;
	a2=CopyTerm(a2);
	alg2_common_n(a2);
	/*alg2_common_t(a2);*/
	alg2_red_cos(a2);
	alg2_red_orth(a2);
	if(FAOutput)
	{
		FAOutput=0;
		alg2_red_1pm5(a2);
		FAOutput=1;
	}
	else
		alg2_red_1pm5(a2);
	
	l=CompoundArgN(a2,5);
	
	if(is_empty_list(l))
		{
		if(verb_charge)
				{
				printf("Empty vertex ");
				WriteVertex(CompoundArg1(a2));
				printf(", assuming charges are zero.\n");
				}
		FreeAtomic(a2);
		return;
		}
		
	for(;l;l=ListTail(l))
		{
		List l1=CompoundArgN(ListFirst(l),3);
		int ng=0, no=0;
		for(;l1;l1=ListTail(l1))
			{
			if(CompoundArg1(ListFirst(l1))==A_DELTA)
				continue;
			if(CompoundArg1(ListFirst(l1))==A_GAMMA)
				{ng++; continue;}
			no++;
			}
		if(ng!=1 || no!=0)
			continue;
		break;
		}
		
	if(l==0)
		{
		printf("Warning: can't determine electric charge from vertex ");
		WriteVertex(CompoundArg1(a2));
		puts(".");
		return;
		}
	
	num=IntegerValue(CompoundArg1(CompoundArg2(a2)));
	num*=IntegerValue(CompoundArg1(ListFirst(l)));
	den=IntegerValue(CompoundArg2(CompoundArg2(a2)));
	cd=gcf(num,den);
	num/=cd;
	den/=cd;
	SetAtomProperty(p1,A_EM_CHARGE,
		MakeCompound2(OPR_DIV,NewInteger(num), NewInteger(den)));
	SetAtomProperty(p2,A_EM_CHARGE,
		MakeCompound2(OPR_DIV,NewInteger(-num), NewInteger(den)));
	if(verb_charge)
				{
				printf("Charges in vertex ");
				WriteVertex(CompoundArg1(a2));
				printf(" : %d/%d and %d/%d\n",num,den,-num,den);
				}
	FreeAtomic(a2);
	
/*	printf("sp: "); WriteTerm(a2); puts("");*/
	
	}

static void set_ve_ch(Term a2, Atom p1, Atom p2, int php)
	{
	List l;
	int num,den,cd;
	Integer mprt, mind;
	a2=CopyTerm(a2);
	alg2_common_n(a2);
	alg2_common_t(a2);
	alg2_red_cos(a2);
	alg2_red_orth(a2);
	if(php==1)
		mprt=NewInteger(2);
	else
		mprt=NewInteger(1);
	mind=ListFirst(CompoundArg1(CompoundArg2(ListNth(CompoundArg1(a2),php))));
	
	l=CompoundArgN(a2,5);
	if(is_empty_list(l))
		{
		if(verb_charge)
				{
				printf("Empty vertex ");
				WriteVertex(CompoundArg1(a2));
				printf(", assuming charges are zero.\n");
				}
		FreeAtomic(a2);
		return;
		}
	while(!is_empty_list(l))
		{
		Term m2;
		List cn,sp;
		m2=ListFirst(l);
		cn=CompoundArg2(m2);
		if(ListLength(cn)!=1)
			goto cnt;
		cn=ListFirst(cn);
		if(CompoundArg1(cn)!=ph_c || CompoundArg2(cn)!=NewInteger(1))
			goto cnt;
		sp=CompoundArgN(m2,3);
		if(ListLength(sp)==2 && (CompoundName((cn=ListFirst(sp)))==A_MOMENT || 
			CompoundName((cn=ListNth(sp,2)))==A_MOMENT) && 
			CompoundArg1(cn)==mprt && ListFirst(CompoundArg2(cn))==mind)
			{
			num=IntegerValue(CompoundArg1(CompoundArg2(a2)));
			num*=IntegerValue(CompoundArg1(m2));
			den=IntegerValue(CompoundArg2(CompoundArg2(a2)));
			cd=gcf(num,den);
			num/=cd;
			den/=cd;
			SetAtomProperty(p1,A_EM_CHARGE,
				MakeCompound2(OPR_DIV,NewInteger(num), NewInteger(den)));
			SetAtomProperty(p2,A_EM_CHARGE,
				MakeCompound2(OPR_DIV,NewInteger(-num), NewInteger(den)));
			if(verb_charge)
				{
				printf("Charges in vertex ");
				WriteVertex(CompoundArg1(a2));
				printf(" : %d/%d and %d/%d\n",num,den,-num,den);
				}
			FreeAtomic(a2);
			return;
			}
	cnt:
		l=ListTail(l);
		}
	
	printf("Warning: can't determine electric charge from vertex ");
	WriteVertex(CompoundArg1(a2));
	puts(".");
	
	
	}
	
static int c_num, c_den;

static void set_num(void)
	{
	c_num=0;
	c_den=1;
	}
	
static void add_num(Term dv)
	{
	int nu, de;
	nu=IntegerValue(CompoundArg1(dv));
	de=IntegerValue(CompoundArg2(dv));
	c_num=c_num*de+nu*c_den;
	c_den*=de;
	}
	
static int get_num(void)
	{
	return c_num;
	}
	
	
void check_em_charge(List lagr)
	{
	List l1,l2;
	
	if(!ph_f)
		return;
		
	l1=lagr;
	while(!is_empty_list(l1))
		{
		Term a2;
		a2=ListFirst(l1);
		l2=CompoundArg1(a2);
		if(ListLength(l2)==3)
			{
			Term p1,p2,pr1;
			Atom at1,at2;
			int php;
			
			if(CompoundArg1(ListFirst(l2))==ph_f)
				{
				p1=ListNth(l2,2);
				p2=ListNth(l2,3);
				php=1;
				}
			else
			if(CompoundArg1(ListNth(l2,2))==ph_f)
				{
				p1=ListNth(l2,1);
				p2=ListNth(l2,3);
				php=2;
				}
			else
			if(CompoundArg1(ListNth(l2,3))==ph_f)
				{
				p1=ListNth(l2,1);
				p2=ListNth(l2,2);
				php=3;
				}
			else
				{
				l1=ListTail(l1);
				continue;
				}
			at1=CompoundArg1(p1);
			at2=CompoundArg1(p2);
			pr1=GetAtomProperty(at1,PROP_TYPE);
			if(is_compound(pr1) && CompoundName(pr1)==OPR_PARTICLE && 
			    ((CompoundArg1(pr1)==at1 && CompoundArg2(pr1)==at2) ||
			     (CompoundArg1(pr1)==at2 && CompoundArg2(pr1)==at1)))
			     {
			     switch(IntegerValue(CompoundArgN(pr1,4)))
			     	{
			     	case 0:
			     		set_sc_ch(a2,at1,at2,php);
			     		break;
			     	case 1:
			     		set_sp_ch(a2,at1,at2,php);
			     		break;
			     	case 2:
			     		set_ve_ch(a2,at1,at2,php);
			     		break;
			     	}
			     }
			}
			     
		l1=ListTail(l1);
		}
		
	l1=lagr;
	while(!is_empty_list(l1))
		{
		List l2;
		Term a2;
		a2=ListFirst(l1);
		if(CompoundArgN(a2,5)==0)
			goto cnt;
		l2=CompoundArg1(a2);
		set_num();
		while(!is_empty_list(l2))
			{
			Term prp;
			prp=GetAtomProperty(CompoundArg1(ListFirst(l2)),PROP_TYPE);
			if(!is_compound(prp) || CompoundName(prp)!=OPR_PARTICLE ||
				CompoundArgN(prp,7)==OPR_MLT)
				{
				Term pp1;
				if(is_compound(prp) && CompoundName(prp)==OPR_FIELD &&
						CompoundArg2(prp)==NewInteger(4))
					prp=CompoundArg1(prp);
				else
					goto cnt;
				pp1=GetAtomProperty(prp,PROP_TYPE);
				if(!is_compound(pp1) || CompoundName(pp1)!=OPR_PARTICLE ||
					CompoundArgN(pp1,7)==OPR_MLT)
					{
					puts("Internal error (phss)");
					goto cnt;
					}
				prp=GetAtomProperty(prp,A_EM_CHARGE);
				}
			else
				prp=GetAtomProperty(CompoundArg1(ListFirst(l2)),A_EM_CHARGE);
			if(prp)
				add_num(prp);
			l2=ListTail(l2);
			}
		if(get_num())
			{
			Term a22;
			a22=CopyTerm(a2);
			alg2_common_n(a22);
			alg2_common_t(a22);
			alg2_red_cos(a22);
			alg2_red_orth(a22);
			if(CompoundArgN(a22,5))
				{
				printf("Warning: vertex ");
				WriteVertex(CompoundArg1(a22));
				printf(" violates EM charge conservation.\n");
				}
			FreeAtomic(a22);
			}
	cnt:
		l1=ListTail(l1);
		}
			
	
	}
	
	
