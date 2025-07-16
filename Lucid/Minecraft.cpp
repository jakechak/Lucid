#include "pch.h"
#include "Minecraft.h"

jclass Minecraft::mc_class = nullptr;
jclass Minecraft::player_class = nullptr;
jclass Minecraft::worldc_class = nullptr;
jclass Minecraft::entity_class = nullptr;
jclass Minecraft::livingentitybase_class = nullptr;
jclass Minecraft::vec_class = nullptr;

jfieldID Minecraft::player_field = nullptr;
jfieldID Minecraft::worldc_field = nullptr;

jobject Minecraft::mc_inst = nullptr;
jobject Minecraft::player = nullptr;

JavaVM* Minecraft::jvm = nullptr;
JNIEnv* Minecraft::env = nullptr;
