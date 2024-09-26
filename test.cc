#include <windows.h>
#include <iostream>

// 一个简单的x86机器码示例，实现的功能是将两个整数相加
// 这个例子是为了说明，实际的机器码可能会更复杂
const unsigned char code[] = {
    0xB8, 0x01, 0x00, 0x00, 0x00, // mov eax, 1
    0x50,                         // push eax
    0xB8, 0x02, 0x00, 0x00, 0x00, // mov eax, 2
    0x50,                         // push eax
    0xC3                          // ret
};

// JIT编译并执行函数
int JITExecute() {
    // 分配内存
    void* execMemory = VirtualAlloc(NULL, sizeof(code),
                                     MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!execMemory) {
        std::cerr << "VirtualAlloc failed!" << std::endl;
        return -1;
    }

    // 复制机器码到分配的内存
    memcpy(execMemory, code, sizeof(code));

    // 修改内存保护属性，使其可执行
    DWORD oldProtect;
    if (!VirtualProtect(execMemory, sizeof(code), PAGE_EXECUTE_READ, &oldProtect)) {
        std::cerr << "VirtualProtect failed!" << std::endl;
        VirtualFree(execMemory, 0, MEM_RELEASE);
        return -1;
    }

    // 类型转换为函数指针并执行
    int(*func)() = reinterpret_cast<int(*)()>(execMemory);
    int result = func(); // 执行JIT编译的代码

    // 释放内存
    VirtualFree(execMemory, 0, MEM_RELEASE);

    return result;
}

int main() {
    int result = JITExecute();
    std::cout << "JIT result: " << result << std::endl; // 应该输出3
    return 0;
}
