#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <libjbc.h>

#include <orbis/SystemService.h>
#include <orbis/libkernel.h>

extern "C" {
uint32_t sceKernelGetCpuTemperature(uint32_t* celsius);
int32_t sceKernelGetSystemSwVersion(OrbisKernelSwVersion* version);

jbc_cred g_Cred, g_RootCreds;

bool freeOfSandbox() {
  int fd = open("/user/.jb_check", 
  O_WRONLY | O_CREAT | O_TRUNC, 0777);
//
  if (fd > 0) {
    unlink("/user/.jb_check");
    close(fd); return true;
  } else return false;
}

#pragma region System
void MountRootDirectories() { return; }

uint32_t GetTemperature() {
  uint32_t celsius;
  sceKernelGetCpuTemperature(&celsius);
  return celsius;
}

void SetFanThreshold(int temp = 80) {
  /*
  int fd = open("/dev/icc_fan", O_RDONLY, 0);

  if (fd <= 0) return;

  int data[10] = {0x00, 0x00, 0x00, 0x00, temp, 0x00, 0x00, 0x00, 0x00, 0x00};

  ioctl(fd, 0xC01C8F07, data);
  close(fd);
  */
}

char* GetFWVersion(bool checkSpoof)
{
    static char version[28];

    if (!checkSpoof) {
        OrbisKernelSwVersion fw_info;
        sceKernelGetSystemSwVersion(&fw_info);

        int i = 0;
        while (i < sizeof(version) - 1 && fw_info.VersionString[i] != '\0') {
            version[i] = fw_info.VersionString[i]; i++;
        }
        version[i] = '\0';
    }

    return version;
}

#pragma endregion

#pragma region Application
void EnterSandbox() {
  if (freeOfSandbox()) 
  jbc_set_cred(&g_Cred);
}

void BreakFromSandbox() {
  if (freeOfSandbox()) return;

  jbc_get_cred(&g_Cred);

  g_RootCreds = g_Cred;

  jbc_jailbreak_cred(&g_RootCreds);
  jbc_set_cred(&g_RootCreds);
}

void ExitApplication() {
  EnterSandbox(); sceSystemServiceLoadExec("exit", 0);
}
#pragma endregion
}