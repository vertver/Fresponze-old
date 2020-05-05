/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2020. All rights reserved.
* Fresponze - fast, simple and modern multimedia sound library
* Apache-2 License
**********************************************************************
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*****************************************************************/
#pragma once
#include "FresponzeTask.h"
#include "tbb/tick_count.h"
#include "tbb/task_group.h"
#include "tbb/global_control.h"

class CTBBTaskManager : public IBaseTaskManager
{
private:
	int TaskThreadCount = 0;
	tbb::global_control* g_control = nullptr;
	tbb::task_group* pTaskGroup = nullptr;

public:
	CTBBTaskManager();
	~CTBBTaskManager();
	fr_i32 GetThreadsCount() override;

	void AddTask(FrTaskFunction* pTaskFunction, void* FunctionContext, fr_i32 TaskPriority, void** ppSynchroniser)  override;
	bool Run(FrTaskFunction* pTaskManagerFunction, void* TaskManagerContext, void* pOtherData) override;
	bool WaitForTask(void* pSynchroniser) override;
};
