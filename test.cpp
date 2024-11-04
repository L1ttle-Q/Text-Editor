#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <chrono>

const int MaxFileNameLength = 50,
          MaxCommandLength = 150;

int main()
{
    fprintf(stdout, "Test num:");
    int num = 0;
    scanf("%d", &num);
    char in_file[MaxFileNameLength],
         out_file[MaxFileNameLength],
         ans_file[MaxFileNameLength];

    snprintf(in_file, MaxFileNameLength, "test/test%d.txt", num);
    snprintf(out_file, MaxFileNameLength, "test/out%d.txt", num);
    snprintf(ans_file, MaxFileNameLength, "test/ans%d.txt", num);

    FILE *fp = fopen(in_file, "r");
    if (!fp)
    {
        fprintf(stdout, "Fail to read the test file!\n");
        return 1;
    }
    else fclose(fp);

    char command_1[MaxCommandLength];
    snprintf(command_1, MaxCommandLength, "./text-editor < %s", in_file);

    auto start = std::chrono::high_resolution_clock::now();

    system(command_1);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed.count() << " milliseconds" << std::endl;

    char command_2[MaxCommandLength];
    #ifdef _WIN64
    snprintf(command_2, MaxCommandLength, "fc %s %s", out_file, ans_file);
    #else
    snprintf(command_2, MaxCommandLength, "diff %s %s && echo \"Files are identical.\"", out_file, ans_file);
    #endif

    system(command_2);

    return 0;
}