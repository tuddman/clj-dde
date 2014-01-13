/*
 * Copyright 2013 www.pretty-tools.com
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
#include "DDEServer-java.h"
#include "DDEServer.h"

extern "C" {

/*
 * Class:     com_pretty_tools_dde_server_DDEServer
 * Method:    nativeStart
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_server_DDEServer_nativeStart (JNIEnv *pEnv, jobject object, jstring service)
{
    WJNIEnv env(pEnv);

    DDEServer* pServer = NULL;
    try
    {
        jobject globalRefObject = NULL;
        ATLTRACE( _T("Obtain reference to DDEServer\n") );
        if (!(globalRefObject = env->NewGlobalRef(object)))
            throw DDEException(IDS_ERROR_CANNOT_CREATE_OBJECT_REFERENCE);

        pServer = new DDEServer(env.GetString(service), env, globalRefObject);
        // start service
        pServer->Start();

        ATLTRACE("Save pointer to DDEServer.%s\n", DDEServer::DDEServerFieldName);
        env.SetLongField(object, DDEServer::DDEServerFieldName, (jlong)pServer);
    }
    catch (DDEMLException* e)
    {
        if (pServer) pServer->Release(env), pServer = NULL;

        ThrowNewDDEMLException(env, *e);
        delete e;
    }
    catch (DDEException& e)
    {
        if (pServer) pServer->Release(env), pServer = NULL;

        ThrowNewDDEException(env, e);
    }
    catch (DDEException* e)
    {
        if (pServer) pServer->Release(env), pServer = NULL;

        ThrowNewDDEException(env, *e);
        delete e;
    }
    catch (...)
    {
        if (pServer) pServer->Release(env), pServer = NULL;

        ThrowNewDDEException(env, GetString(IDS_ERROR_UNKNOWN));
    }
}

/*
 * Class:     com_pretty_tools_dde_server_DDEServer
 * Method:    nativeStop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_server_DDEServer_nativeStop(JNIEnv *pEnv, jobject object)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEServer* pServer = DDEServer::GetDDEServer(env, object);

        ATLTRACE("Reset DDEServer.%s\n", DDEServer::DDEServerFieldName);
        env.SetLongField(object, DDEServer::DDEServerFieldName, 0);

        ATLTRACE( _T("Release DDEServer\n") );
        pServer->Release(env);
    }
    _CATCH_ALL_
}
/*
 * Class:     com_pretty_tools_dde_server_DDEServer
 * Method:    nativeNotifyClients
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_pretty_1tools_dde_server_DDEServer_nativeNotifyClients(JNIEnv *pEnv, jobject object, jstring topic, jstring item)
{
    WJNIEnv env(pEnv);

    _TRY_
    {
        DDEServer* pDDEServer = DDEServer::GetDDEServer(env, object);

        pDDEServer->PostAdvise(env.GetString(topic), env.GetString(item));
    }
    _CATCH_ALL_
}

}

