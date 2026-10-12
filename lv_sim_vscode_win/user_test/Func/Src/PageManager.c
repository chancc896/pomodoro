#include "../Inc/PageManager.h"
#include "../../GUI_App/Screens/Inc/ui_HomePage.h"
#include "../../GUI_App/Screens/Inc/ui_MenuPage.h"

PageStack_t PageStack;

static void page_stack_init(PageStack_t* stack) {
    stack->top = 0;
}

static uint8_t page_stack_push(PageStack_t* stack, Page_t* page) {
    if (stack->top >= MAX_DEPTH)
		return -1;
    stack->pages[stack->top++] = page;
	return 0;
}

static uint8_t page_stack_pop(PageStack_t* stack) {
    if (stack->top <= 0)
		return -1;
    stack->pages[--stack->top]->deinit();
    return 0;
}

static uint8_t page_stack_is_empty(const PageStack_t* stack) {
    return stack->top == 0;
}

static Page_t* get_top_page(PageStack_t* stack) {
    // 检查栈是否为空
    if (stack->top == 0) {
        return NULL; // 如果栈为空，返回NULL
    }

    // 返回栈顶页面的指针
    return stack->pages[stack->top - 1];
}


/**
 * 获取当前页面（栈顶页面）的指针。
 *
 * @param NULL
 * @return 返回当前页面的指针，如果栈为空则返回NULL。
 */
Page_t* Page_Get_NowPage(void) {
    return get_top_page(&PageStack);
}


/**
 * back to previous page
 *
 * @param NULL
 * @return NULL
 */
void Page_Back(void) {
	if (page_stack_is_empty(&PageStack)) {
        // 栈为空时，不应发生
        return;
    }

    // 弹出当前页面
    page_stack_pop(&PageStack);

    if (page_stack_is_empty(&PageStack)) {
        // 如果栈为空，入栈并切换到MenuPage
        page_stack_push(&PageStack, &Page_Home);
        page_stack_push(&PageStack, &Page_Menu);
        Page_Menu.init();
        lv_scr_load_anim(*Page_Menu.page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, true);
    } else {
        // 切换到上一个页面
        Page_t *previous_page = PageStack.pages[PageStack.top - 1];
        previous_page->init();
        lv_scr_load_anim(*previous_page->page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, true);
    }
}

/**
 * back to bottom page home page
 *
 * @param NULL
 * @return NULL
 */
void Page_Back_Bottom(void) {

    if (page_stack_is_empty(&PageStack)) {
        // 栈为空时，不应发生
        return;
    }

    // 弹出除栈底的所有页面
    while(PageStack.top > 1)
        page_stack_pop(&PageStack);
    PageStack.pages[PageStack.top - 1]->init(); // 初始化新页面
    lv_scr_load_anim(*PageStack.pages[PageStack.top - 1]->page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, true); // 加载并应用动画
}

/**
 * Load a new page to stack top
 *
 * @param newPage Page_t a new page
 * @return NULL
 */
void Page_Load_Dir(Page_t *newPage, lv_scr_load_anim_t anim) {
	// 检查堆栈是否已满
    if (PageStack.top >= MAX_DEPTH - 1) {
        return;
    }

    // 如果堆栈非空，反初始化当前页面
    if (PageStack.top > 0) {
        PageStack.pages[PageStack.top - 1]->deinit();
    }

    // 将新页面推入堆栈
    page_stack_push(&PageStack, newPage);
    newPage->init();
    lv_scr_load_anim(*newPage->page_obj, anim, 500, 0, true);  // 方向由参数决定
}

void Page_Load(Page_t *newPage) {
    // 默认向右（原行为，给菜单等所有老页面用）
    Page_Load_Dir(newPage, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
}

void Page_Replace_Dir(Page_t *newPage, lv_scr_load_anim_t anim)
{
    if(PageStack.top == 0)//如果栈为空，退为普通压栈
    {
        page_stack_push(&PageStack, newPage);
        newPage->init();
        lv_scr_load_anim(*newPage->page_obj, anim, 500, 0 ,true);
        return;
    }
    //将栈顶的页面销毁
    PageStack.pages[PageStack.top - 1]->deinit();
    //将新页面压入栈顶
    PageStack.pages[PageStack.top - 1] = newPage;

    newPage->init();
    lv_scr_load_anim(*newPage->page_obj, anim, 500, 0 ,true);
}

/**
 * initialize the page manager & start home page
 *
 * @param NULL
 * @return NULL
 */
void Pages_init(void) {
    page_stack_init(&PageStack);
    page_stack_push(&PageStack, &Page_Home);
    Page_Home.init();
    lv_disp_load_scr(*Page_Home.page_obj);
}
