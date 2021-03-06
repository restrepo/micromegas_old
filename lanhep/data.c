#include <string.h>
#include <stdlib.h>
#include "lanhep.h"

/*#define DATA_DBG*/

/*********

  1 - atoms
  2-30 functors
  31- floats
  32-47 - compounds
  48-63 - lists
  64 - * labels
  
  
***********/

union fpoint
	{
	double d;
	struct 	{
		union fpoint *next;
		Float f;
		} p;
	};
	

	
static union fpoint *f_next_free=NULL;	
static union fpoint *fbuffers[256];
static int f_blocks=0, f_dtf=0, f_ftd=0;
	
Float NewFloat(double d)
	{
	union fpoint *p;
	Float f;
	if(f_next_free==NULL)
		{
		int i;
		if(f_blocks==256)
			{  puts("Internal error (too much floats or memory lack)."); exit(0); }
		p=fbuffers[f_blocks]=(union fpoint*)malloc(sizeof(union fpoint)*1000);
		if(p==NULL)
			{  puts("Internal error (lack space for float)."); exit(0); }
		for(i=0;i<1000;i++)
			{
			p[i].p.next=f_next_free;
			f_next_free=p+i;
			p[i].p.f=0x1f000000+f_blocks*0x10000+i;
			}
		f_blocks++;
		}
	p=f_next_free;
	f_next_free=f_next_free->p.next;
	f=p->p.f;
	p->d=d;
	f_dtf++;
	return f;
	}
	
double FloatValue(Float f)
	{
	union fpoint *p;
	int bno,bpos;
	bno=f&0xff0000;
	bno/=0x10000;
	bpos=f&0xffff;
	p=fbuffers[bno]+bpos;
	return p->d;
	}


union COMPOpoint *COMPO_next_free=NULL;	
union COMPOpoint *COMPO_buffers[4096];
int COMPO_blocks=0, COMPO_tall=0, COMPO_tfree=0;
int LABEL_count = 0;


/*
#ifndef USE_INLINE

#define __inline
#include "data.ci"
#undef __inline

#endif
*/

#define __inline

__inline Integer NewInteger(long l)
	{
	Integer i;
	i=l-1073741824;
	if(i>=0)
		{  puts("Internal error (integer out of range)."); 
		/*abort();*/
		   return NewInteger(0); }
	return i;
	}

__inline long IntegerValue(Integer i)
	{
	return i+1073741824;
	}

/*static int newfu[30];*/


__inline Functor NewFunctor(Atom name, int arity)
	{
	if(arity<1 || arity >29)
		{  puts("Internal error (arity out of range)."); exit(0); }
	/*newfu[arity]++;*/
	return (name&0xffffff) + (arity+1)*0x1000000;
	}

/*	
__inline Atom 	FunctorName(Functor f)
	{
	return (f & 0xffffff) + 0x1000000;
	}
	
__inline int 	FunctorArity(Functor f)
	{
	return (f/0x1000000)-1;
	}
*/
			
__inline void COMPO_free(Compound c)
	{
	union COMPOpoint *p;
	int bno,bpos;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	p->p.this_a=c;
	p->p.next=COMPO_next_free;
	p->data.next_a=1;
	COMPO_next_free=p;
	COMPO_tfree++;
	return;
	}
	
	
__inline union COMPOpoint *COMPO_alloc(Compound *cco)
	{
	union COMPOpoint *p;
	Compound co;
	if(COMPO_next_free==NULL)
		{
		int i;
		if(COMPO_blocks==4096)
			{  puts("Internal error (too much terms)."); exit(0); }
		i=sizeof(union COMPOpoint)*65500;
		p=COMPO_buffers[COMPO_blocks]=(union COMPOpoint*)malloc(i);
		if(p==NULL)
			{  puts("Internal error (lack space for terms)."); exit(0); }
		for(i=0;i<65500;i++)
			{
			p[i].p.next=COMPO_next_free;
			COMPO_next_free=p+i;
			p[i].p.this_a=0x20000000+COMPO_blocks*0x10000+i;
			}
		COMPO_blocks++;
		}
	p=COMPO_next_free;
	COMPO_next_free=COMPO_next_free->p.next;
	co=p->p.this_a;
	*cco=co;
	COMPO_tall++;
	return p;
	}

__inline Compound  NewCompound(Functor fu)
	{
	union COMPOpoint *p;
	Compound c;
	int arity;
	
	arity=FunctorArity(fu);
	p=COMPO_alloc(&c);
	p->data.a[0]=fu;
	p->data.a[1]=p->data.a[2]=0;
	p->data.next_a=0; p->data.next_p=NULL;
	arity-=2;
	while(arity>0)
		{
		Compound c1;
		union COMPOpoint *p1;
		p1=COMPO_alloc(&c1);
		p->data.next_a=c1;
		p->data.next_p=p1;
		p1->data.a[0]=p1->data.a[1]=p1->data.a[2]=p1->data.next_a=0;
		p1->data.next_p=NULL;
		p=p1;
		arity-=3;
		}
	return c;
	}

	
	
/*	
__inline Functor CompoundFunctor(Compound c)
	{
	union COMPOpoint *p;
	int bno,bpos;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	return p->data.a[0];
	}
*/
			
__inline void SetCompoundName(Compound c, Atom name)
	{
	union COMPOpoint *p;
	int bno,bpos, ar;
	Functor f;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	f=p->data.a[0];
	ar=FunctorArity(f);
	p->data.a[0]=NewFunctor(name,ar);
	}
/*
__inline Term CompoundArg1(Compound c)
	{
	union COMPOpoint *p;
	int bno,bpos;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	return p->data.a[1];
	}

__inline Term CompoundArg2(Compound c)
	{
	union COMPOpoint *p;
	int bno,bpos;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	return p->data.a[2];
	}
*/
__inline Term CompoundArgN(Compound c, int arg)
	{
	union COMPOpoint *p;
	int bno,bpos;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	while(arg>2)
		{ p=p->data.next_p; arg-=3; }
	return p->data.a[arg];
	}

__inline Term ConsumeCompoundArg(Compound c, int arg)
	{
	union COMPOpoint *p;
	int bno,bpos;
	Term ret;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;

	while(arg>2)
		{ p=p->data.next_p; arg-=3; }
	ret=p->data.a[arg];
	p->data.a[arg]=0;
	return ret;
	}	


__inline void SetCompoundArg(Compound c, int arg, Term t)
	{
	union COMPOpoint *p;
	int bno,bpos;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;

	while(arg>2)
		{ p=p->data.next_p; arg-=3; }
	/*if(p->data.a[arg]!=0) FreeAtomic(p->data.a[arg]);*/
	p->data.a[arg]=t;
	}
	

__inline Compound MakeCompound(Atom name, int arity)
	{
	return NewCompound(NewFunctor(name,arity));
	}

__inline Compound MakeCompound1(Atom name, Term arg1)
    {
    Term tt;
    tt=NewCompound(NewFunctor(name,1));
    SetCompoundArg(tt,1,arg1);
    return tt;
    }

__inline Compound MakeCompound2(Atom name, Term arg1, Term arg2)
    {
    Term tt;
    tt=NewCompound(NewFunctor(name,2));
    SetCompoundArg(tt,1,arg1);
    SetCompoundArg(tt,2,arg2);
    return tt;
    }

__inline Compound MakeCompound3(Atom name, Term arg1, Term arg2, Term arg3)
    {
    Term tt;
    tt=NewCompound(NewFunctor(name,3));
    SetCompoundArg(tt,1,arg1);
    SetCompoundArg(tt,2,arg2);
    SetCompoundArg(tt,3,arg3);
    return tt;
    }

/*
__inline Atom CompoundName(Compound c)
	{
	return FunctorName(CompoundFunctor(c));
	}

__inline int CompoundArity(Compound c)
	{
	return FunctorArity(CompoundFunctor(c));
	}
*/
			 
__inline int is_atomic(Atomic a)
	{
	return is_atom(a) || is_integer(a) || is_float(a) || 
		is_label(a) || a==0;
	}

__inline int is_float(Atomic a)
	{
	return (a&0xff000000)==0x1f000000;
	}

__inline int is_integer(Atomic a)
	{
	return a<0;
	}

__inline int is_atom(Atomic a)
	{
	return (a&0xff000000)==0x01000000;
	}

__inline int is_functor(Atomic a)
	{
	int b;
	b=a/0x1000000;
	return (b>1 && b<31);
	}

__inline int is_compound(Term a)
	{
	return a/0x10000000 == 2;
	}

__inline int is_label(Term a)
	{
	return a/0x10000000 >3 ;
	}


__inline Atomic NewLabel(void)
	{
	return ((++LABEL_count)&0x3fffffff)+0x40000000;
	}

__inline int   LabelValue(Atomic l)
	{
	return l-0x40000000;
	}


void FreeCompound(Compound c)
	{
	union COMPOpoint *p,*p1;
	int bno,bpos,arity;
	Compound c1;
	bno=c&0xfff0000;
	bno/=0x10000;
	bpos=c&0xffff;
	p=COMPO_buffers[bno]+bpos;
	arity=FunctorArity(p->data.a[0]);
	do
		{
		p1=p->data.next_p;
		c1=p->data.next_a;
		FreeAtomic(p->data.a[0]);
		FreeAtomic(p->data.a[1]);
		FreeAtomic(p->data.a[2]);
		COMPO_free(c);
		p=p1;
		c=c1;
		}   while(p!=NULL && c!=0);
	}
		
	

void FreeAtomic(Atomic a)
	{
	long type;
	if(a<0) return;
	type = a&0xff000000;
	type = type/0x1000000;
	if(type==31)
		{
		union fpoint *p;
		int bno,bpos;
		bno=a&0xff0000;
		bno/=0x10000;
		bpos=a&0xffff;
		p=fbuffers[bno]+bpos;
		p->p.f=a;
		p->p.next=f_next_free;
		f_next_free=p;
		f_ftd++;
		return;
		}
	if(type>=32 && type<48)
		{
		FreeCompound(a);
		return;
		}
	if(type>=48 && type<64)
		{
		FreeList(a);
		return;
		}
		
	}

Term CopyTerm(Term t)
     {
     char ty;
     ty=AtomicType(t);
     switch(ty)
         {
         case 'i':
         case 'a':
         case 'e':
         case '?':
         case 'u':
         case 'L':
              return t;
         case 'f':
              {
              double f;
              f=FloatValue(t);
              return NewFloat(f);
              }
         case 'c':
              {
              Term tt;
              int i,ar;
              ar=CompoundArity(t);
              tt=MakeCompound(CompoundName(t),ar);
              for(i=1;i<=ar;i++)
                  SetCompoundArg(tt,i,CopyTerm(CompoundArgN(t,i)));
              return tt;
              }
         case 'l':
              {
              List li,ll;
              li=NewList();
              ll=t;
              while(!is_empty_list(ll))
                  {
                  li=AppendLast(li,CopyTerm(ListFirst(ll)));
                  ll=ListTail(ll);
                  }
              return li;
              }
         }
     return 0;
     }

	
char AtomicType(Atomic a)
	{
	long type;
	if(a<0) return 'i';
	type = a&0xff000000;
	type = type/0x1000000;
	if(type==1)
		return 'a';
	if(type==31)	/* 0xff */
		return 'f';
	if(type>=32 && type<48)   /* 0xfe */
		return 'c';
	if(type>=48 && type<64)  /*  0xfd */ 
		return 'l';
	if(type>=64)  /*  0xfc */
		return 'L';
	if(type>1 && type<31)
		return 'u';
	if(a==0)
		return 'e';
	return '?';
	}
	
		
void AtomStatistics(void)
	{
	/*int i;*/
	printf("Atoms: %d       , %d a->s  , %d s->a\n",ATOM_count,ATOM_tscount,
				ATOM_stcount);
	printf("Terms: %d blocks use %d Kbytes, %d allocs ( %d remain) \n",
		COMPO_blocks,COMPO_blocks*sizeof(union COMPOpoint)*64,
		COMPO_tall,COMPO_tall-COMPO_tfree);
	printf("Float: %d blocks, %d allocs, %d frees\n",f_blocks,f_dtf,f_ftd);
	printf("Labels:          %d allocs\n",LABEL_count);
	/*for(i=1;i<30;i++)
		printf("arity %2d: %d allocs\n",i,newfu[i]);*/
	}

void AtomStat1(void)
	{
	printf("%d atoms, ",ATOM_count);
	printf("%d terms (%d blocks), ",COMPO_tall-COMPO_tfree,COMPO_blocks);
	}

long TermMemory(void)
	{
	return (long)COMPO_blocks*sizeof(union COMPOpoint)*64;
	}
	
int EqualTerms(Term t1, Term t2)
	{
	if(t1==t2)
		return 1;
	if(is_compound(t1) && is_compound(t2) && 
		CompoundFunctor(t1) == CompoundFunctor(t2))
		{
		int i,a;
		a=CompoundArity(t1);
		for(i=1;i<=a;i++)	
			if(!EqualTerms(CompoundArgN(t1,i),CompoundArgN(t2,i)))
				return 0;
		return 1;
		}
	if(is_list(t1) && is_list(t2) && ListLength(t1)==ListLength(t2))
		{
		List l1,l2;
		l1=t1;
		l2=t2;
		while(!is_empty_list(l1))
			{
			if(!EqualTerms(ListFirst(l1),ListFirst(l2)))
				return 0;
			l1=ListTail(l1);
			l2=ListTail(l2);
			}
		return 1;
		}
	return 0;
	}
			
			
