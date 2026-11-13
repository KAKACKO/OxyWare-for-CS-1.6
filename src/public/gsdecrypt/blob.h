
/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
*
*	This program is licensed under the MIT license. By downloading, copying,
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a
*	copy of this software and associated documentation files (the "Software"),
*	to deal in the Software without restriction, including without limitation
*	the rights to use, copy, modify, merge, publish, distribute, sublicense,
*	and/or sell copies of the Software, and to permit persons to whom the
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included
*	in all copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
*	IN THE SOFTWARE.
*/
#ifndef BLOB_H
#define BLOB_H
#pragma once

#include <vector>

#define BLOB_ALGORITHM_MAGIC 0x12345678

struct blob_info_t
{
	char	m_szPath[10];
	char	m_szDescribe[32];
	char	m_szCompany[22];
	DWORD	m_dwMagic;
};

struct blob_hdr_t
{
	DWORD	m_dwCheckSum;
	WORD	m_wSectionCount;
	DWORD	m_dwExportPoint;
	DWORD	m_dwImageBase;
	DWORD	m_dwEntryPoint;
	DWORD	m_dwImportTable;
};

struct blob_section_t
{
	DWORD	m_dwVirtualAddress;
	DWORD	m_dwVirtualSize;


	DWORD	m_dwDataSize;
	DWORD	m_dwDataAddress;

	BOOL	m_bIsSpecial;
};

#endif