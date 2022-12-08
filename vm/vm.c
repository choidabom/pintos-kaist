/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"

/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */
void vm_init(void)
{
	vm_anon_init();
	vm_file_init();
#ifdef EFILESYS /* For project 4 */
	pagecache_init();
#endif
	register_inspect_intr();
	/* DO NOT MODIFY UPPER LINES. */
	/* TODO: Your code goes here. */
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type
page_get_type(struct page *page)
{
	int ty = VM_TYPE(page->operations->type);
	switch (ty)
	{
	case VM_UNINIT:
		return VM_TYPE(page->uninit.type);
	default:
		return ty;
	}
}

/* Helpers */
static struct frame *vm_get_victim(void);
static bool vm_do_claim_page(struct page *page);
static struct frame *vm_evict_frame(void);

/* Create the pending page object with initializer. If you want to create a
 * page, do not create it directly and make it through this function or
 * `vm_alloc_page`. */
bool vm_alloc_page_with_initializer(enum vm_type type, void *upage, bool writable,
									vm_initializer *init, void *aux)
{

	ASSERT(VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current()->spt;

	/* Check wheter the upage is already occupied or not. */
	if (spt_find_page(spt, upage) == NULL)
	{
		/* TODO: Create the page, fetch the initialier according to the VM type,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */

		/* TODO: Insert the page into the spt. */
	}
err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL.
=> 함수의 인자로 넘겨진 SPT에서로부터 가상 주소(VA)와 대응되는 페이지 구조체를 찾아서 반환한다. */
struct page *
spt_find_page(struct supplemental_page_table *spt UNUSED, void *va UNUSED)
{
	/* 지역변수 page를 만들어서 인자로 받은 va를 넣어줘. 일단 구색을 맞춰주는 것임 ! */
	struct page p;
	/* TODO: Fill this function. */
	/* */
	// return page;
}

/* Insert PAGE into spt with validation.
=> 함수의 인자로 넘겨진 SPT에 페이지 구조체를 삽입한다. SPT에 가상주소(Virtual Address)가 존재하지 않는지 검사해야 한다. */
bool spt_insert_page(struct supplemental_page_table *spt UNUSED,
					 struct page *page UNUSED)
{
	int succ = false;
	/* TODO: Fill this function. */

	return succ;
}

void spt_remove_page(struct supplemental_page_table *spt, struct page *page)
{
	vm_dealloc_page(page);
	return true;
}

/* Get the struct frame, that will be evicted. */
static struct frame *
vm_get_victim(void)
{
	struct frame *victim = NULL;
	/* TODO: The policy for eviction is up to you. */

	return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.*/
static struct frame *
vm_evict_frame(void)
{
	struct frame *victim UNUSED = vm_get_victim();
	/* TODO: swap out the victim and return the evicted frame. */

	return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/
static struct frame *
vm_get_frame(void)
{
	struct frame *frame = NULL;
	/* TODO: Fill this function. */

	ASSERT(frame != NULL);		 // 유저풀에서 잘 가져왔는지 확인
	ASSERT(frame->page == NULL); // 어떤 page도 맵핑되어 있지 않아야 함
	return frame;
}

/* Growing the stack. */
static void
vm_stack_growth(void *addr UNUSED)
{
}

/* Handle the fault on write_protected page */
static bool
vm_handle_wp(struct page *page UNUSED)
{
}
/*
	(https://casys-kaist.github.io/pintos-kaist/project3/introduction.html)
	현재 핀토스는 page fault 발생 시 프로세스를 kill한다.
	하지만 이제 page fault가 발생하면 물리디스크를 탐색하여 page frame을 할당해야 한다.

	1. 유효한 주소라면:
	메모리에 존재하지 않는다면(not-present) spt를 탐색해 해당 주소에 들어가야 할 컨텐츠를 물리 메모리에서 찾아서 해당 데이터를 가져온다
	또는 파일에서 읽어와야 하는 데이터이거나 스왑 영역에서 가져와야 하는 데이터이거나 아니면 그냥 새로운 페이지를 할당해야 되는 경우일 수 있다.(all-zero page)

	2. 유효한 주소가 아니라면:
	커널 주소거나 유효한 유저주소가 아니거나 적절한 permission이 없다면 프로세스를 kill한다.
*/

/* Return true on success */
bool vm_try_handle_fault(struct intr_frame *f UNUSED, void *addr UNUSED,
						 bool user UNUSED, bool write UNUSED, bool not_present UNUSED)
{
	struct supplemental_page_table *spt UNUSED = &thread_current()->spt;
	struct page *page = NULL;
	/* TODO: Validate the fault */
	/* TODO: Your code goes here */
	/*
	1. 보조 페이지 테이블에서 폴트가 발생한 페이지를 찾는다. -> spt_find_page (구현해야함)
	2. 만일 메모리 참조가 유효하다면 보조 페이지 엔트리를 사용해서 페이지에 들어가는 데이터를 찾는다.
	3. 페이지를 저장하기 위해 프레임을 획득
	4. 데이터를 파일 시스템이나 스왑에서 읽어오거나, 0으로 초기화하는 등의 방식으로 만들어서 프레임으로 가져온다.
	5. 가상주소에 대한 페이지 테이블 엔트리가 물리 페이지를 가리키도록 지정한다. => vm_do_claim_page */
	return vm_do_claim_page(page); // vm(page) -> RAM(frame)의 연결관계가 설정되어 있지 않아 page fault가 발생할 때 이 연결관계를 설정하여 준다.
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void vm_dealloc_page(struct page *page)
{
	destroy(page);
	free(page);
}

/* Claim the page that allocate on VA. */
bool vm_claim_page(void *va UNUSED)
{
	struct page *page = NULL;
	/* TODO: Fill this function */

	return vm_do_claim_page(page);
}

/* Claim the PAGE and set up the mmu. */
static bool
vm_do_claim_page(struct page *page)
{
	struct frame *frame = vm_get_frame();

	/* Set links */
	frame->page = page;
	page->frame = frame;

	/* TODO: Insert page table entry to map page's VA to frame's PA. */

	return swap_in(page, frame->kva);
}

/* Initialize new supplemental page table
 supplemental page table을 초기화한다. userprog/process.c의 initd 함수로 새로운 프로세스가 시작하거나 process.c의 __do_fork로 자식 프로세스가 생성될 때 위의 함수가 호출된다.  */
void supplemental_page_table_init(struct supplemental_page_table *spt UNUSED)
{
	/* SPT 안에다가 해시 테이블을 만들었기 때문에 다음과 같이 작성 !!
	=> &spt->spt_hash_table */
	hash_init(&spt->spt_hash_table, page_hash, page_less, NULL);
}

/* Copy supplemental page table from src to dst */
bool supplemental_page_table_copy(struct supplemental_page_table *dst UNUSED,
								  struct supplemental_page_table *src UNUSED)
{
}

/* Free the resource hold by the supplemental page table */
void supplemental_page_table_kill(struct supplemental_page_table *spt UNUSED)
{
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */
}
