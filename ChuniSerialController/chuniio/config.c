#include <windows.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "chuniio/config.h"


void chuni_io_config_load(
        struct chuni_io_config *cfg,
        const wchar_t *filename)
{
    // wchar_t key[16];
    // int i;

    assert(cfg != NULL);
    assert(filename != NULL);

    cfg->vk_test = GetPrivateProfileIntW(L"io3", L"test", VK_F1, filename);
    cfg->vk_service = GetPrivateProfileIntW(L"io3", L"service", VK_F2, filename);
    cfg->vk_coin = GetPrivateProfileIntW(L"io3", L"coin", VK_F3, filename);
    // cfg->vk_ir = GetPrivateProfileIntW(L"io3", L"ir", VK_SPACE, filename);

    // for (i = 0 ; i < 32 ; i++) {
    //     swprintf_s(key, _countof(key), L"cell%i", i + 1);
    //     cfg->vk_cell[i] = GetPrivateProfileIntW(
    //             L"slider",
    //             key,
    //             chuni_io_default_cells[i],
    //             filename);
    // }
}
