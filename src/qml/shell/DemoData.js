.pragma library
// Placeholder demo content for the shell mockups. Later specs replace these with
// real models (notebook tree -> #5, editor -> #7, outline -> #9, search/tags -> #11/#12).

var TREE = [
    { depth: 0, icon: "folder", label: "算法", open: true },
    { depth: 1, icon: "folder", label: "二叉树", open: true },
    { depth: 2, icon: "md", label: "二叉树的最大路径和.md", selected: true },
    { depth: 2, icon: "md", label: "右视图.md" },
    { depth: 2, icon: "md", label: "层序遍历.md" },
    { depth: 1, icon: "folder", label: "动态规划", open: false },
    { depth: 1, icon: "folder", label: "图论", open: false },
    { depth: 0, icon: "folder", label: "读书笔记", open: true },
    { depth: 1, icon: "md", label: "人月神话.md" },
    { depth: 1, icon: "md", label: "设计模式.md" },
    { depth: 0, icon: "folder", label: "日记", open: false },
    { depth: 0, icon: "file", label: "README.md", muted: true }
];

var TABS = [
    { label: "二叉树的最大路径和.md", active: true, dirty: true },
    { label: "右视图.md", active: false, dirty: false },
    { label: "README.md", active: false, dirty: false }
];

var OUTLINE = [
    { label: "二叉树的最大路径和", level: 0, active: true },
    { label: "思路", level: 1, active: false },
    { label: "复杂度分析", level: 1, active: false },
    { label: "相关题目", level: 1, active: false },
    { label: "扩展：路径计数", level: 1, active: false }
];

var TAGS = ["#算法", "#二叉树", "#DFS"];

var NOTES = [
    { title: "二叉树的最大路径和", snippet: "给定一个二叉树的根节点 root，返回其最大路径和。对每个节点计算最大贡献值…", meta: "6月9日 · 612 字", active: true, fav: false },
    { title: "二叉树的右视图", snippet: "想象自己站在树的右侧，按从顶到底的顺序，返回从右侧能看到的节点值。", meta: "6月7日 · 388 字", active: false, fav: false },
    { title: "层序遍历", snippet: "BFS 逐层展开。用队列维护当前层，记录每层节点数即可分层输出。", meta: "6月5日 · 520 字", active: false, fav: false },
    { title: "人月神话 · 读书笔记", snippet: "没有银弹。软件的根本复杂度无法通过工具消除，只能管理。", meta: "6月2日 · 1.2k 字", active: false, fav: true },
    { title: "设计模式速记", snippet: "策略、观察者、装饰器——组合优于继承，面向接口编程。", meta: "5月28日 · 904 字", active: false, fav: false }
];

// Workbench (C) data.
var NAV_ITEMS = [
    { icon: "inbox", label: "收件箱", count: "3" },
    { icon: "file", label: "全部笔记", count: "218" },
    { icon: "star", label: "收藏", count: "" },
    { icon: "clock", label: "最近", count: "" }
];

// notebook colors resolved against Theme at use site (key -> token name).
var NOTEBOOKS = [
    { colorRole: "accent", label: "算法", active: true },
    { colorRole: "lime", label: "读书笔记", active: false },
    { colorRole: "amber", label: "日记", active: false },
    { colorRole: "pink", label: "项目 · Markly", active: false }
];

var NAV_TAGS = ["#算法", "#DFS", "#二叉树", "#复习"];

var PROPS = {
    status: "已完成",
    tags: ["算法", "DFS", "困难"],
    updated: "2026-06-09 · 412 字"
};

var BACKLINKS = [
    { label: "路径计数问题", colorRole: "lime" },
    { label: "DFS 模板总结", colorRole: "accent" }
];

// Editor source lines for A (each line is an array of {t, role} spans).
// role maps to a Theme color token; "" => text.
var EDITOR_LINES = [
    [{ t: "# ", role: "faint" }, { t: "二叉树的最大路径和", role: "heading", h1: true }],
    [],
    [{ t: "给定一个二叉树的根节点 ", role: "text" }, { t: "`root`", role: "codeInline" }, { t: "，返回其 ", role: "text" }, { t: "最大路径和", role: "text", bold: true }, { t: "。", role: "text" }],
    [],
    [{ t: "> 路径 被定义为一条从树中任意节点出发，沿父—子连接走过的节点序列。", role: "dim", italic: true }],
    [],
    [{ t: "## ", role: "faint" }, { t: "思路", role: "heading", h2: true }],
    [],
    [{ t: "1. 对每个节点，计算左右子树的最大贡献值。", role: "text" }],
    [{ t: "2. 负贡献直接舍弃（取 ", role: "text" }, { t: "`max(0, gain)`", role: "codeInline" }, { t: "）。", role: "text" }],
    [{ t: "3. 用 ", role: "text" }, { t: "`node.val + left + right`", role: "codeInline" }, { t: " 更新全局答案。", role: "text" }],
    [],
    [{ t: "```cpp", role: "faint" }],
    [{ t: "int dfs(TreeNode* node) {", role: "codeInk" }],
    [{ t: "    if (!node) return 0;", role: "codeInk" }],
    [{ t: "    int l = max(0, dfs(node->left));", role: "codeInk" }],
    [{ t: "    int r = max(0, dfs(node->right));", role: "codeInk" }],
    [{ t: "    ans = max(ans, node->val + l + r);", role: "codeInk" }],
    [{ t: "    return node->val + max(l, r);", role: "codeInk" }],
    [{ t: "}", role: "codeInk" }],
    [{ t: "```", role: "faint" }],
    [],
    [{ t: "时间复杂度 ", role: "text" }, { t: "`O(n)`", role: "codeInline" }, { t: "，空间复杂度 ", role: "text" }, { t: "`O(h)`", role: "codeInline" }, { t: "。", role: "text" }]
];
