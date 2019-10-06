#pragma once
#include "FresponzeTypes.h"

class IBaseEffect : public IBaseInterface
{
protected:

public:
	virtual bool GetEffectCategory(fr_i32& EffectCategory) = 0;			// use BasePluginCategory enum here

	virtual bool GetVariablesCount(fr_i32& CountOfVariables) = 0;
	virtual bool GetVariableDescription(fr_string128& DescriptionString) = 0;
	virtual bool SetVariable(fr_i32 VariableIndex, fr_f32 VariableValue) = 0;
	virtual bool GetVariable(fr_f32 VariableIndex, fr_f32& VariableValue) = 0;

	virtual bool Process(fr_i32** ppData, fr_i32 Frames) = 0;
}; 

typedef struct
{
	EffectNodeStruct* pNext;
	IBaseEffect* pEffect;  
	void* pModuleHandle;  

} EffectNodeStruct;
