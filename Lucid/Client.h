#pragma once

#include <jni.h>
#include <vector>
#include <memory>

#include "Module.h"

class Client
{
public:
	static jclass mc_class;
	static jclass player_class;
	static jclass worldc_class;
	static jclass entity_class;
	static jclass livingentitybase_class;
	static jclass vec_class;

	static jfieldID player_field;
	static jfieldID worldc_field;

	static jobject mc_inst;
	static jobject player;

	static JavaVM* jvm;
	static JNIEnv* env;

	static std::vector<std::unique_ptr<Module>> modules;

	static jstring client_name;

	static void Init();
};