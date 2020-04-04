package Client;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.Iterator;
import java.util.Set;

/**
 * Client selector를 이용해 ReadFromServer 실행 (non block) thread를 추가하여 WriteToServer
 * 클라이언트 종료
 * 
 */

public class Client extends Thread {

    public static String address = "127.0.0.1";
    public static int port = 12000;
    InetSocketAddress connectAddress = new InetSocketAddress(address, port);
    public static SocketChannel sc;
    Selector selector;
    CharsetDecoder decoder = null;

    // 버퍼 만들기
    ByteBuffer buffer = ByteBuffer.allocate(1024);

    public void run() {

        startClient();
    }

    private void startClient() {
        System.out.println("before Client is connecting...");

        try {
            selector = Selector.open();
            sc = SocketChannel.open(connectAddress);
            sc.configureBlocking(false);
            sc.register(selector, SelectionKey.OP_READ);

            Charset charset = Charset.forName("UTF-8");
            decoder = charset.newDecoder();

            new WriteToServer(sc, buffer).start();

            // selector를 이용해 read
            // while(selector.select() > 0) {
            for (;;) {

                int keyCount = selector.select();

                if (keyCount == 0) {
                    continue;
                }

                Set<SelectionKey> selectedKeys = selector.selectedKeys();
                Iterator<SelectionKey> iter = selectedKeys.iterator();
                System.out.println("chenck loop1");

                while (iter.hasNext()) {

                    SelectionKey selectionKey = (SelectionKey) iter.next();

                    System.out.println("chenck loop2");

                    if (!selectionKey.isValid()) {
                        continue;
                    }

                    if (selectionKey.isReadable()) {
                        
                        readFromServer(selectionKey);
                    }

                    iter.remove();
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void readFromServer(SelectionKey selectionKey) throws IOException {
        
        SocketChannel selectedChannel = (SocketChannel) selectionKey.channel();

        int size = selectedChannel.read(buffer);

        if (size == -1) {
            selectedChannel.close();
            selectionKey.cancel();
            selector.close();
            System.out.print("Client is disconnected!!");
            return;
        }

        buffer.flip();
        String data = decoder.decode(buffer).toString();
        System.out.println(data);

        // System.out.println("read result");
        // printState(buffer);
        buffer.clear();
        
    }

    private void printState(ByteBuffer buffer2) {
        System.out.print("\tposition: " + buffer.position() + ", ");
        System.out.print("\tlimit: " + buffer.limit() + ", ");
        System.out.println("\tcapacity: " + buffer.capacity());

    }
}
