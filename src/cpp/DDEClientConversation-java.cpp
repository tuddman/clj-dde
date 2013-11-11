/*
 * Copyright 2008 www.pretty-tools.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "StdAfx.h"
#include "DDEClientConversation-java.h"
#include "DDEClientConversation.h"

extern "C" {

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeConnect
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeConnect(JNIEnv *pEnv, jobject object, jstring service, jstring topic)
{
    WJNIEnv env(pEnv);

    DDEClientConversation* pConversation = NULL;
    try
    {
        jobject globalRefObject = NULL;
        ATLTRACE( _T("Obtain reference to DDEClientConversation\n") );
        if (!(globalRefObject = env->NewGlobalRef(object)))
            throw DDEException(IDS_ERROR_CANNOT_CREATE_OBJECT_REFERENCE);

        pConversation = new DDEClientConversation(env, globalRefObject);
        // connect to service
        pConversation->Connect(env.GetString(service), env.GetString(topic));

        ATLTRACE("Save pointer to DDEClientConversation.%s\n", DDEClientConversation::DDEClientConversationFieldName);
        env.SetLongField(object, DDEClientConversation::DDEClientConversationFieldName, (jlong)pConversation);
    }
    catch (DDEMLException* e)
    {
        if (pConversation) pConversation->Release(env), pConversation = NULL;

        ThrowNewDDEMLException(env, *e);
        delete e;
    }
    catch (DDEException& e)
    {
        if (pConversation) pConversation->Release(env), pConversation = NULL;

        ThrowNewDDEException(env, e);
    }
    catch (DDEException* e)
    {
        if (pConversation) pConversation->Release(env), pConversation = NULL;

        ThrowNewDDEException(env, *e);
        delete e;
    }
    catch (...)
    {
        if (pConversation) pConversation->Release(env), pConversation = NULL;

        ThrowNewDDEException(env, GetString(IDS_ERROR_UNKNOWN));
    }
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeDisconnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeDisconnect(JNIEnv *pEnv, jobject object)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        ATLTRACE("Reset DDEClientConversation.%s\n", DDEClientConversation::DDEClientConversationFieldName);
        env.SetLongField(object, DDEClientConversation::DDEClientConversationFieldName, 0);

        // disconnect and destroy
        pConversation->Disconnect();

        ATLTRACE( _T("Release DDEClientConversation\n") );
        pConversation->Release(env);
    }
    _CATCH_ALL_
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeExecute
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeExecute__Ljava_lang_String_2(JNIEnv *pEnv, jobject object, jstring command)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        pConversation->Execute(env.GetString(command));
    }
    _CATCH_ALL_
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeExecute
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeExecute___3B(JNIEnv *pEnv, jobject object, jbyteArray command)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        pConversation->Execute(env.GetByteArray(command));
    }
    _CATCH_ALL_
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativePoke
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativePoke__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv *pEnv, jobject object, jstring item, jstring data)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        pConversation->Poke(env.GetString(item), env.GetString(data));
    }
    _CATCH_ALL_
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativePoke
 * Signature: (Ljava/lang/String;[B)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativePoke__Ljava_lang_String_2_3BI(JNIEnv *pEnv, jobject object, jstring item, jbyteArray data, jint wFmt)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        pConversation->Poke(env.GetString(item), env.GetByteArray(data), (UINT)wFmt);
    }
    _CATCH_ALL_
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeRequest
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeRequest(JNIEnv *pEnv, jobject object, jstring item)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        CString strData = pConversation->Request(env.GetString(item));

        return env.NewString(strData);
    }
    _CATCH_ALL_

    return NULL;
}


/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeStartAdvice
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeStartAdvice(JNIEnv *pEnv, jobject object, jstring item)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        pConversation->StartAdvice(env.GetString(item));
    }
    _CATCH_ALL_
}

/*
 * Class:     com_pretty_tools_dde_client_DDEClientConversation
 * Method:    nativeStopAdvice
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_client_DDEClientConversation_nativeStopAdvice(JNIEnv *pEnv, jobject object, jstring item)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEClientConversation* pConversation = DDEClientConversation::GetDDEClient(env, object);

        pConversation->StopAdvice(env.GetString(item));
    }
    _CATCH_ALL_
}
  
}
