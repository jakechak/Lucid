#pragma once

#include <jni.h>

class Minecraft
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

	static void Init()
	{
		mc_class = env->FindClass("ave");
		player_class = env->FindClass("bew");
		worldc_class = env->FindClass("bdb");
		entity_class = env->FindClass("pk");
		livingentitybase_class = env->FindClass("pr");
		vec_class = env->FindClass("aui");

		player_field = env->GetFieldID(mc_class, "h", "Lbew;");
		worldc_field = env->GetFieldID(mc_class, "f", "Lbdb;");

		jmethodID GetMinecraft = env->GetStaticMethodID(mc_class, "A", "()Lave;");
		mc_inst = env->CallStaticObjectMethod(mc_class, GetMinecraft);
	}
};