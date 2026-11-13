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

#ifndef HOOKTESTS_H
#define HOOKTESTS_H
#pragma once


#ifdef _DEBUG

#endif


class TestableHook
{
public:
	virtual void test_hook() { };

	virtual void default_test_hook() { };
};

class CHookTests
{
public:
	DECL_BASIC_CLASS(CHookTests);

public:
	void execute_tests();

	void run_seh_protected_block(const std::string& name, const std::function<bool()>& code_to_be_ran, bool is_default = false);

public:
	void add_for_testing(const std::string& hook_type, TestableHook* hook)
	{
		m_testable_hooks[hook_type].push_back(hook);
	}

private:
	std::unordered_map<std::string, std::vector<TestableHook*>> m_testable_hooks;
};

#endif