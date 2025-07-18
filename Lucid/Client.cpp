#include "pch.h"
#include "Client.h"
#include "Reach.h"
#include "Autoclicker.h"

jclass Client::mc_class = nullptr;
jclass Client::player_class = nullptr;
jclass Client::worldc_class = nullptr;
jclass Client::entity_class = nullptr;
jclass Client::livingentitybase_class = nullptr;
jclass Client::vec_class = nullptr;

jfieldID Client::player_field = nullptr;
jfieldID Client::worldc_field = nullptr;

jobject Client::mc_inst = nullptr;
jobject Client::player = nullptr;

JavaVM* Client::jvm = nullptr;
JNIEnv* Client::env = nullptr;

jstring Client::client_name = nullptr;

std::vector<std::unique_ptr<Module>> Client::modules;

void Client::Init()
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

	client_name = env->NewStringUTF("Lucid");

	modules.push_back(std::make_unique<Reach>());
	modules.push_back(std::make_unique<Autoclicker>());
}
