package com.Baz;

public class Foo {

  public static final String value = "Called from Foo.java";

    private String service;
    /** Name of the topic on which a conversation is to be established. */
    private String topic;
    /** Maximum amount of time, in milliseconds, that the client will wait for a response from the server application in a synchronous transaction. */


  public Foo()
    {}

  public void BazFn()
    {}

  public synchronized void connect(String service, String topic)
    {
        System.out.println("connect has been called.");
        this.service = service;
        this.topic = topic;
    }


  }







