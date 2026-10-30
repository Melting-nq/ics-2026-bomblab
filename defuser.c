#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>
//新添加头文件string
#include <string.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t high = (uint32_t)(challenge >> 32);

    uint32_t middle = (uint32_t)(challenge >> 16);

    uint32_t low = (uint32_t)challenge;

    uint32_t result1 = (middle + 0xc699) ^ high;

    uint32_t result2 = (3 * low) ^ (high + 0x71b5);

    uint32_t answer = 5 * (result2 - result1) + 0x96fa;

    values[0] = answer;

    count = 1;

   return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = 3 * (uint32_t)((challenge >> 24) & 31);

    for (int i = 1; i < 6; i++) {

        uint32_t offset = (uint32_t)((challenge >> (i + 20)) & 15);

        values[i] = 5 * (values[i - 1] + (uint32_t)i) - offset;

    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t index = (uint32_t)(challenge & 7);
    uint32_t a = (uint32_t)(challenge >> 3);
    uint32_t b = (uint32_t)(challenge >> 32);
    uint32_t x = a ^ b;
    uint32_t result = 0;
    switch (index) {
        case 0:
            result = 5 * x + 0xf0fc;
            break;
        case 1:
            result = (x ^ 0x8a21) + (x >> 1);
            break;
        case 2:
            result = 5 * x + 0x3f408;
            break;
        case 3:
            result = 5 * x + 0x2cc27;
            break;
        case 4:            result = (x ^ 0x6853) + (x >> 1);
            break;
        case 5:
            result = (4 * x) ^ 0x4373;
            break;
        case 6:
            result = (2 * x) ^ 0x7bf3;
            break;
        case 7:
            result = 3 * x + 0xd3c8;
            break;
    }
    values[0] = index;
    values[1] = result;
    return write_answer(out, cap, values, count);
}

//这是递归函数主体
static uint32_t solve_p4_walk(uint32_t target,uint32_t low,uint32_t high,uint32_t salt) {
    uint32_t mid = low + (high - low) / 2;
    if (target == mid) {
        return (mid + 0xd1) ^ salt;
    }
    uint32_t key = mid + salt;
    if (target < mid) {
        uint32_t result = solve_p4_walk(target, low, mid - 1, salt);
        return ((3 * result) ^ key) ^ 0xd;
    } else {
        uint32_t result = solve_p4_walk(target, mid + 1, high, salt);
        return ((3 * result) ^ key) ^ 0x1;
    }
}
//这是phase4的主体代码
int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t target = (uint32_t)(challenge & 0x7f);

    uint32_t salt =
        (uint32_t)(challenge >> 7)
        ^ (uint32_t)(challenge >> 32);
    values[0] = solve_p4_walk(target, 0, 127, salt);
    return write_answer(out, cap, values, count);
}

//这是对phase5代码的改进，将节点整理为数组
typedef struct {
    uint32_t value;
    uint32_t tag;
} P5Node;

static const P5Node p5_nodes[] = {
    {0xe387, 0x0222},
    {0xab32, 0x0608},
    {0x0596, 0x0a58},
    {0x1a18, 0x0411},
    {0xc4a3, 0x007e},
    {0x408c, 0x0827},
    {0x0c6e, 0x0559},
    {0xbf54, 0x0372},
    {0x22e1, 0x0733},
    {0x6b22, 0x0976},
    {0x3713, 0x0b43},
    {0x0f9e, 0x0154}
};
int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t index = (uint32_t)(challenge & 7);
    uint32_t steps = (uint32_t)((challenge >> 3) & 3);
    uint32_t result =
        (uint32_t)(challenge >> 6)
        ^ (uint32_t)(challenge >> 32)
        ^ 0x834c;
    uint32_t tag = 0;
    for (uint32_t i = 0; i <= steps; i++) {
        P5Node node = p5_nodes[index + i];
        result = 5 * (result + node.value) ^ node.tag;
        tag = node.tag;
    }
    values[0] = result;
    values[1] = tag;
    return write_answer(out, cap, values, count);
}

//六个函数定义
static uint32_t p6_op_0(uint32_t x) {
    return (x ^ 0x219b) + 8 * x;
}

static uint32_t p6_op_1(uint32_t x) {
    return (x ^ (x >> 1)) + 0x4a1d;
}

static uint32_t p6_op_2(uint32_t x) {
    return (x + 0xf01e) ^ (x >> 3);
}

static uint32_t p6_op_3(uint32_t x) {
    return (x >> 5) | (x << 27);
}

static uint32_t p6_op_4(uint32_t x) {
    return ((x << 1) | (x >> 31)) ^ 0x2b29;
}

static uint32_t p6_op_5(uint32_t x) {
    return (2 * x + 0xd36) ^ (3 * x);
}
static uint32_t (*const p6_ops[8])(uint32_t) = {
    p6_op_4,
    p6_op_0,
    p6_op_5,
    p6_op_0,
    p6_op_5,
    p6_op_1,
    p6_op_2,
    p6_op_3
};
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    // 从 challenge 中提取初始计算值
    uint32_t x =
        (uint32_t)(challenge >> 6)
        ^ (uint32_t)(challenge >> 32);
    // 低三位决定第一次调用哪个函数
    uint32_t index1 = (uint32_t)(challenge & 7);
    // 第 3～5 位决定第二次调用哪个函数
    uint32_t index2 = (uint32_t)((challenge >> 3) & 7);
    // 第一次间接函数调用
    uint32_t result = p6_ops[index1](x);
    // 两次函数调用之间的异或运算
    result ^= 0xedf2;
    // 第二次间接函数调用
    result = p6_ops[index2](result);
    // 最终偏移
    result += 0x913a;
    values[0] = result;
    return write_answer(out, cap, values, count);
}

//secret中无比复杂的函数
static uint32_t solve_secret_mix(uint32_t x) {
    uint32_t result = (x << 5) | (x >> 27);
    result += 0x2d05;
    result ^= 0xf2ea;
    return result;
}
int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    // 1. 第一个整数：context 的最低两位

    uint32_t a = (uint32_t)(context & 3);

    // 2. challenge 高低 32 位异或

    uint32_t x =

        (uint32_t)(challenge >> 32)

        ^ (uint32_t)challenge;
    // 3. context 高低 32 位异或
    uint32_t y =
        (uint32_t)(context >> 32)
        ^ (uint32_t)context;
    // 4. 第二个整数
    uint32_t b =
        solve_secret_mix(x ^ y)
        ^ (a * 0x2d05)
        ^ (uint32_t)(context >> 16);
    // 5. 第三个整数
    uint32_t c =

        b ^ (y + a + 0xf2ea + (x >> 5));
    // 6. 前32个字节填充空格
    if (cap <= 32) {
        return 1;
    }

    memset(out, ' ', 32);

    // 7. 在第33个字节开始写入 OPEN 和三个整数
    int size = snprintf(
        out + 32,
        cap - 32,
        "OPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
        a, b, c
    );

    // 8. 检查输出是否成功
    if (size < 0 || (size_t)size >= cap - 32) {
        return 1;
    }

    return 0;
}
