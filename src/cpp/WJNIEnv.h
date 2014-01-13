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
#pragma once

/**
 * JNIEnv wrapper/helper.
 *
 * @author Alexander Kozlov
 */
class WJNIEnv
{
private:
    JNIEnv *env;

public:
    WJNIEnv(JNIEnv *pJNIEnv)
        : env(pJNIEnv)
    {
    }

    JNIEnv* operator -> () const
    {
        return env;
    }

    operator JNIEnv* () const
    {
        return env;
    }

    WJNIEnv &operator = (JNIEnv *pJNIEnv)
    {
        env = pJNIEnv;
        return *this;
    }

    JNIEnv** operator & ()
    {
        return &env;
    }

    CString GetString(jstring jstrValue)
    {
        if (!jstrValue)
            return CString();

        const jsize iLength = env->GetStringLength(jstrValue);
        if (!iLength)
            return CString();

        const jchar *pjchValue = env->GetStringChars(jstrValue, NULL);
        if (!pjchValue)
            return CString();

        CStringW strValue;
        memcpy(strValue.GetBuffer(iLength), (const wchar_t *)pjchValue, sizeof(WCHAR)*iLength);
        strValue.ReleaseBuffer(iLength);

        env->ReleaseStringChars(jstrValue, pjchValue);

#ifdef _UNICODE
        return strValue;
#else
        return (CString)strValue;
#endif // _UNICODE
    }

    jstring NewString(LPCSTR pszValue)
    {
        return pszValue ? NewString((CStringW)pszValue) : NULL;
    }

    jstring NewString(LPCWSTR pszValue)
    {
        return pszValue ? env->NewString((jchar*)pszValue, (jsize)wcslen(pszValue)) : NULL;
    }

    jstring NewString(CString* pstrValue)
    {
        return pstrValue ? NewString(*pstrValue) : NULL;
    }

    CSimpleArray<BYTE> GetByteArray(jbyteArray jbyteArrayValue)
    {
        if (!jbyteArrayValue)
            return CSimpleArray<BYTE>();

        jsize nLength = env->GetArrayLength(jbyteArrayValue);
        if (!nLength)
            return CSimpleArray<BYTE>();

        jbyte* pData = env->GetByteArrayElements(jbyteArrayValue, 0);
        if (!pData)
            return CSimpleArray<BYTE>();

        CSimpleArray<BYTE> arData;

        for (jsize i = 0; i < nLength; ++i)
            arData.Add(pData[i]);

        env->ReleaseByteArrayElements(jbyteArrayValue, pData, JNI_ABORT);

        return arData;
    }

    jbyteArray NewByteArray(const CSimpleArray<BYTE> &ar)
    {
        jbyteArray jbyteArray = env->NewByteArray(ar.GetSize());
        if (!jbyteArray || ar.GetSize() == 0)
            return jbyteArray;

        jbyte* pData = env->GetByteArrayElements(jbyteArray, 0);
        if (!pData)
            return 0;

        memcpy(pData, ar.GetData(), ar.GetSize());

        env->ReleaseByteArrayElements(jbyteArray, pData, 0 /*copy back the content and free the elems buffer */);

        return jbyteArray;
    }
    
    jint GetIntField(jobject object, const char *name)
    {
        jclass clazz = env->GetObjectClass(object);
        jfieldID fid = env->GetFieldID(clazz, name, "I");

        jint result = fid ? env->GetIntField(object, fid) : 0;

        env->DeleteLocalRef(clazz);

        return result;
    }

    void SetIntField(jobject object, const char *name, jint iValue)
    {
        jclass clazz = env->GetObjectClass(object);
        jfieldID fid = env->GetFieldID(clazz, name, "I");

        if (fid) env->SetIntField(object, fid, (jint)iValue);
    
        env->DeleteLocalRef(clazz);
    }

    jlong GetLongField(jobject object, const char *name)
    {
        jclass clazz = env->GetObjectClass(object);
        jfieldID fid = env->GetFieldID(clazz, name, "J");

        jlong result = fid ? env->GetLongField(object, fid) : 0;
    
        env->DeleteLocalRef(clazz);

        return result;
    }

    void SetLongField(jobject object, const char *name, jlong lValue)
    {
        jclass clazz = env->GetObjectClass(object);
        jfieldID fid = env->GetFieldID(clazz, name, "J");

        if (fid) env->SetLongField(object, fid, lValue);
    
        env->DeleteLocalRef(clazz);
    }

    void CallVoidMethod(jobject object, const char *name)
    {
        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "()V");

        if (mid) env->CallVoidMethod(object, mid);

        env->DeleteLocalRef(clz);
    }

    void CallVoidMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2, LPCTSTR pszParam3)
    {
        jint pushed = env->PushLocalFrame(5);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);
            jstring jstrParam3 = NewString(pszParam3);

            env->CallVoidMethod(object, mid, jstrParam1, jstrParam2, jstrParam3);
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
    }

    jobject NewObject(jclass clazz, int nParam1, LPCTSTR pszParam2)
    {
        jmethodID mid = env->GetMethodID(clazz, "<init>", "(ILjava/lang/String;)V");
        if (!mid) // constructor not found
            return NULL;

        jstring jstrParam2 = NewString(pszParam2);

        jobject newObject = env->NewObject(clazz, mid, (jint)nParam1, jstrParam2);

        env->DeleteLocalRef(jstrParam2);

        return newObject;
    }
    
    jboolean CallBooleanMethod(jobject object, const char *name, LPCTSTR pszParam1)
    {
        jboolean result = JNI_FALSE;

        jint pushed = env->PushLocalFrame(3);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;)Z");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);

            result = env->CallBooleanMethod(object, mid, jstrParam1);
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        return result;
    }
        
    jboolean CallBooleanMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2)
    {
        jboolean result = JNI_FALSE;

        jint pushed = env->PushLocalFrame(4);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;)Z");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);

            result = env->CallBooleanMethod(object, mid, jstrParam1, jstrParam2);
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        
        return result;
    }
        
    jboolean CallBooleanMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2, jint iParam3)
    {
        jboolean result = JNI_FALSE;

        jint pushed = env->PushLocalFrame(4);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;I)Z");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);

            result = env->CallBooleanMethod(object, mid, jstrParam1, jstrParam2, iParam3);
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        
        return result;
    }
        
    jboolean CallBooleanMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2, LPCTSTR pszParam3)
    {
        jboolean result = JNI_FALSE;

        jint pushed = env->PushLocalFrame(5);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);
            jstring jstrParam3 = NewString(pszParam3);

            result = env->CallBooleanMethod(object, mid, jstrParam1, jstrParam2, jstrParam3);
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        
        return result;
    }
        
    jboolean CallBooleanMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2, const CSimpleArray<BYTE>& arParam3, jint iParam4)
    {
        jboolean result = JNI_FALSE;

        jint pushed = env->PushLocalFrame(5);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;[BI)Z");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);
            jbyteArray jarrayParam3 = NewByteArray(arParam3);

            result = env->CallBooleanMethod(object, mid, jstrParam1, jstrParam2, jarrayParam3, iParam4);
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        
        return result;
    }

    CString CallStringMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2)
    {
        CString strResult;

        jint pushed = env->PushLocalFrame(5);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);

            strResult = GetString((jstring)env->CallObjectMethod(object, mid, jstrParam1, jstrParam2));
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        
        return strResult;
    }
        
    CSimpleArray<BYTE> CallBytesMethod(jobject object, const char *name, LPCTSTR pszParam1, LPCTSTR pszParam2, jint iParam3)
    {
        CSimpleArray<BYTE> arResult;

        jint pushed = env->PushLocalFrame(5);

        jclass clz = env->GetObjectClass(object);
        jmethodID mid = env->GetMethodID(clz, name, "(Ljava/lang/String;Ljava/lang/String;I)[B");
        if (mid) // otherwise method not found
        {
            jstring jstrParam1 = NewString(pszParam1);
            jstring jstrParam2 = NewString(pszParam2);

            arResult = GetByteArray((jbyteArray)env->CallObjectMethod(object, mid, jstrParam1, jstrParam2, iParam3));
        }

        if (pushed == 0) env->PopLocalFrame(NULL);
        
        return arResult;
    }
};
