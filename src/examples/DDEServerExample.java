/*
 * Copyright 2013 www.pretty-tools.com. All rights reserved.
 */

import com.pretty_tools.dde.ClipboardFormat;
import com.pretty_tools.dde.DDEException;
import com.pretty_tools.dde.DDEMLException;
import com.pretty_tools.dde.client.DDEClientConversation;
import com.pretty_tools.dde.client.DDEClientEventListener;
import com.pretty_tools.dde.server.DDEServer;

import java.util.Arrays;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * DDE Server Example.
 *
 * @author Alexander Kozlov
 */
public class DDEServerExample
{
    private static final String SERVICE = "MyServer";
    private static final String TOPIC = "MyTopic";
    private static final String MY_ADVISE_ITEM = "MyAdvise";
    private static final String MY_REQUEST_ITEM = "MyRequest";
    private static final String MY_POKE_ITEM = "MyPoke";
    private static final List<String> ITEMS = Arrays.asList(MY_ADVISE_ITEM, MY_REQUEST_ITEM, MY_POKE_ITEM);

    public static void main(String[] args)
    {
        try
        {
            // event to wait disconnection
            final CountDownLatch eventStop = new CountDownLatch(1);
            final AtomicInteger num = new AtomicInteger(1);

            final DDEServer server = new DDEServer(SERVICE) {
                @Override
                protected boolean isTopicSupported(String topicName)
                {
                    return TOPIC.equalsIgnoreCase(topicName);
                }

                @Override
                protected boolean isItemSupported(String topic, String item, int uFmt)
                {
                    return isTopicSupported(topic)
                           && ITEMS.contains(item)
                           && (uFmt == ClipboardFormat.CF_TEXT.getNativeCode() || uFmt == ClipboardFormat.CF_UNICODETEXT.getNativeCode());
                }

                @Override
                protected boolean onExecute(String command)
                {
                    System.out.println("onExecute(" + command + ")");

                    if ("stop".equalsIgnoreCase(command))
                        eventStop.countDown();

                    return true;
                }

                @Override
                protected boolean onPoke(String topic, String item, String data)
                {
                    System.out.println("onPoke(" + topic + ", " + item + ", " + data + ")");

                    return true;
                }

                @Override
                protected boolean onPoke(String topic, String item, byte[] data, int uFmt)
                {
                    System.out.println("onPoke(" + topic + ", " + item + ", " + data + ", " + uFmt + ")");

                    return false; // we do not support it
                }

                @Override
                protected String onRequest(String topic, String item)
                {
                    System.out.println("onRequest(" + topic + ", " + item + ")");

                    return item + " data " + num;
                }

                @Override
                protected byte[] onRequest(String topic, String item, int uFmt)
                {
                    System.out.println("onPoke(" + topic + ", " + item + ", " + uFmt + ")");

                    return null; // we do not support it
                }
            };

            System.out.println("Starting...");
            server.start();

            final Timer timer = new Timer();
            timer.schedule(new TimerTask() {
                @Override
                public void run()
                {
                    num.incrementAndGet();
                    try {
                        System.out.println("notify clients");
                        server.notifyClients(TOPIC, MY_ADVISE_ITEM);
                    } catch (DDEException e) {
                        System.out.println("DDEClientException: " + e.getMessage());
                        cancel();
                    }
                }
            }, 1000L, 500L);

            System.out.println("Waiting for stop...");
            eventStop.await();
            System.out.println("stopping...");
            timer.cancel();
            server.stop();
            System.out.println("Exit from thread");
        }
        catch (DDEMLException e)
        {
            System.out.println("DDEMLException: 0x" + Integer.toHexString(e.getErrorCode()) + " " + e.getMessage());
        }
        catch (DDEException e)
        {
            System.out.println("DDEClientException: " + e.getMessage());
        }
        catch (Exception e)
        {
            System.out.println("Exception: " + e);
        }
    }
}