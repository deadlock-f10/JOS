// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	if(((err & FEC_WR) == 0) == 0)
		panic("pgfault: not a write attempt\n");
	if((uvpt[PGNUM(addr)] & PTE_COW) == 0)
		panic("pgfault: attempt to access a non-COW page");
	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	int  ret = sys_page_alloc(thisenv->env_id,PFTEMP,(PTE_W|PTE_U|PTE_P));
	if(ret < 0)
		panic("pgfault: %e",ret);
	memcpy(PFTEMP,(void *)ROUNDDOWN(addr,PGSIZE),PGSIZE);
	if((ret = sys_page_map(thisenv->env_id,PFTEMP,thisenv->env_id,(void *)ROUNDDOWN(addr,PGSIZE),(PTE_W|PTE_U|PTE_P))) < 0)
		panic("pgfault: %e",ret);
	if((ret = sys_page_unmap(thisenv->env_id,PFTEMP)) < 0)
		panic("pgfault: %e",ret);
	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int ret;
	envid_t thisid = sys_getenvid(); 
	void * addr = (void *)(pn<<PTXSHIFT);
	if(((uvpt[pn] & PTE_P) == PTE_P)|((uvpt[pn] & PTE_COW) == PTE_COW))
	{
		if(( ret = sys_page_map(thisid,addr,envid,(void *)(pn<<PTXSHIFT),(PTE_COW|PTE_P|PTE_U))) < 0)
			panic("duppage : %e\n",ret);
		if(( ret = sys_page_map(thisid,addr,thisenv->env_id,(void *)(pn<<PTXSHIFT),(PTE_COW|PTE_P|PTE_U))) < 0)
			panic("duppage : %e\n",ret);
	}
	else
		if((ret = sys_page_map(thisenv->env_id,addr,envid,(void *)(pn<<PTXSHIFT),(PTE_P|PTE_U))) < 0)
			panic("duppage : %e\n",ret);

	// LAB 4: Your code here.
//	panic("duppage not implemented");
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	envid_t childID,thisid = sys_getenvid();
	set_pgfault_handler(pgfault);
	int ret;
	childID = sys_exofork();
	if( childID < 0)
		panic("fork : %e\n",childID);
	else if( childID == 0)
	{
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	unsigned addr = UTOP - 2*PGSIZE;
	for(;addr >= UTEXT ; addr -= PGSIZE){
		if(((uvpd[PDX(addr)] & PTE_P) == PTE_P) & ((uvpt[PGNUM(addr)] & PTE_P) == PTE_P))
			duppage(childID,PGNUM(addr));
	}
	if((ret = sys_page_alloc(childID,(void *)(UXSTACKTOP - PGSIZE),(PTE_W | PTE_P | PTE_U))) < 0)
		panic("fork : %e",ret);
	extern void _pgfault_upcall(void);
	sys_env_set_pgfault_upcall(childID,_pgfault_upcall);
	if((ret = sys_env_set_status(childID,ENV_RUNNABLE)) < 0)
		panic("fork : %e",ret);
	return childID;
	panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
