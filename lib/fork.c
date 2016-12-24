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
	if ((err & FEC_WR) == 0) {
		panic("faulting access was not a write");
	}
	uint32_t pgnum = PGNUM(addr);
	if ((uvpt[pgnum] & PTE_COW) == 0) {
		panic("faulting not to a copy-on-write page");
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	// LAB 4: Your code here.
	r = sys_page_alloc(0, PFTEMP, PTE_P | PTE_U | PTE_W);	// 0 means current envid
	if (r != 0) {
		panic("pgfault: fail to allocate a page");
	}
	void * addr_aligned = ROUNDDOWN(addr,PGSIZE);
	memcpy(PFTEMP, addr_aligned, PGSIZE);
	r = sys_page_map(0, PFTEMP, 
					 0, addr_aligned, 
					 PTE_P | PTE_U | PTE_W);
	if (r != 0) {
		panic("pgfault: fail to map page");
	}
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)	recursively duppage.
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	void * addr = (void *)(pn * PGSIZE);
	// mark it that can be directly modified to enable sharing
	if (uvpt[pn] & PTE_SHARE) {
		r = sys_page_map(thisenv->env_id, addr,
						 envid, addr,
						 uvpt[pn] & PTE_SYSCALL);
		if (r != 0) {
			panic("duppage: fail to map page");
		}
	}
	else if ((uvpt[pn] & (PTE_W | PTE_COW)) != 0) {
		// first map into child's address space
		r = sys_page_map(thisenv->env_id, addr,
						 envid, addr,
						 PTE_COW | PTE_U | PTE_P);
		if (r != 0) {
			panic("duppage: fail to map page");
		}
		// then map into parent's address space
		r = sys_page_map(thisenv->env_id, addr,
						 thisenv->env_id, addr,
						 PTE_COW | PTE_U | PTE_P);
		if (r != 0) {
			panic("duppage: fail to map page");
		}
	} else {	// not writable and COW
		r = sys_page_map(thisenv->env_id, addr,
						 envid, addr,
						 PTE_U | PTE_P);
		if (r != 0) {
			panic("duppage: fail to map page");
		}
	}
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
	set_pgfault_handler(pgfault);
	
	envid_t envid = sys_exofork();
	if (envid < 0) {
		panic("fork: fail to create child process");
	}
	if (envid == 0) {	// in child process
		thisenv = &envs[ENVX(sys_getenvid())];	// update thisenv
		return 0;	// child return 0
	}
	// in parent process
	uint32_t pgnum;
	for (pgnum = 0; pgnum < PGNUM(UTOP-PGSIZE); pgnum++) {
		if ((uvpd[PDX(pgnum << PTXSHIFT)] & PTE_P) == 0)	// ptd doesn't exist, do nothing
			continue;
		if ((uvpt[pgnum] & PTE_P) == PTE_P) {
			duppage(envid, pgnum);
		}
	}
	// allocate exception stack
	int r;
	if ((r = sys_page_alloc(envid, (void *)(UXSTACKTOP - PGSIZE), PTE_P | PTE_U | PTE_W)) < 0) {
		panic("fork: fail to alloc page for exception stack");
	}
	// set pgfault handler for child
	if ((r = sys_env_set_pgfault_upcall(envid, thisenv->env_pgfault_upcall)) < 0) {
		panic("fork: fail to set pgfault handler for child");
	}
	// set env status for child
	if ((r = sys_env_set_status(envid, ENV_RUNNABLE)) < 0) {
		panic("fork: fail to set child's env status");
	}
	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
