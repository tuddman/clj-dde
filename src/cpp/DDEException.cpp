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
#include "DDEException.h"

#define DDEEXCEPTION        "com/pretty_tools/dde/DDEException"
#define DDEMLEXCEPTION      "com/pretty_tools/dde/DDEMLException"

/**
 * Throw new DDEException.
 */
void ThrowNewDDEException(WJNIEnv env, LPCTSTR pszMessage)
{
    jclass clazz = env->FindClass(DDEEXCEPTION);

    if (clazz) env->ThrowNew(clazz, (CStringA)pszMessage);
}

void ThrowNewDDEException(WJNIEnv env, const DDEException& ex)
{
    ThrowNewDDEException(env, ex.GetMessage());
}

void ThrowNewDDEMLException(WJNIEnv env, const DDEMLException& ex)
{
    jclass clazz = env->FindClass(DDEMLEXCEPTION);
    if (!clazz) // NoClassDefFoundError
        return;

    jobject jthr = env.NewObject(clazz, ex.GetCode(), ex.GetMessage());

    if (jthr) env->Throw((jthrowable)jthr);
}