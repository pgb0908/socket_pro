package test;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;

public class Server implements Runnable {
    private InetSocketAddress listenAddress;
    // �޽����� �������� �����Ѵ�.
    private static char CR = (char) 0x0D;
    private static char LF = (char) 0x0A;

    // ip�� port ����
    public Server(String address, int port) {
        listenAddress = new InetSocketAddress(address, port);
    }

    // Thread ����.
    public void run() {
        // ������ ����
        try (Selector selector = Selector.open()) {
            // ä�� ����
            try (ServerSocketChannel serverChannel = ServerSocketChannel.open()) {
                // non-Blocking ����
                serverChannel.configureBlocking(false);
                // ���� ip, port ����
                serverChannel.socket().bind(listenAddress);
                // ä�ο� accept ��� ����
                serverChannel.register(selector, SelectionKey.OP_ACCEPT);
                // �����Ͱ� ���� ���.
                while (selector.select() > 0) {
                    // ������ Ű �¸� �����´�.
                    Iterator<SelectionKey> keys = selector.selectedKeys().iterator();
                    // Ű�� ������..
                    while (keys.hasNext()) {
                        SelectionKey key = keys.next();
                        //Ű �¿��� ����.
                        keys.remove();
                        if (!key.isValid()) {
                            continue;
                        }
                        // ������ ���..
                        if (key.isAcceptable()) {
                            this.accept(selector, key);
                            // ������ ���..
                        } else if (key.isReadable()) {
                            this.receive(selector, key);
                            // �߽��� ���..
                        } else if (key.isWritable()) {
                            this.send(selector, key);
                        }
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // ���ӽ� ȣ�� �Լ�..
    private void accept(Selector selector, SelectionKey key) {
        try {
            // Ű ä���� �����´�.
            ServerSocketChannel serverChannel = (ServerSocketChannel) key.channel();
            // accept�� �ؼ� Socket ä���� �����´�.
            SocketChannel channel = serverChannel.accept();
            channel.configureBlocking(false);
            // ���� ���
            Socket socket = channel.socket();
            SocketAddress remoteAddr = socket.getRemoteSocketAddress();
            System.out.println("Connected to: " + remoteAddr);
            // ���� Socket ������ ���Ǵ� Buffer;
            StringBuffer sb = new StringBuffer();
            sb.append("Welcome server!\r\n>");
            // Socket ä���� channel�� �۽� ����Ѵ�
            channel.register(selector, SelectionKey.OP_WRITE, sb);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // ���Ž� ȣ�� �Լ�..
    private void receive(Selector selector, SelectionKey key) {
        try {
            // Ű ä���� �����´�.
            SocketChannel channel = (SocketChannel) key.channel();
            // ä�� Non-blocking ����
            channel.configureBlocking(false);
            // ���� ���
            Socket socket = channel.socket();
            // Byte ���� ����
            ByteBuffer buffer = ByteBuffer.allocate(1024);
            // ***������ ����***
            int size = channel.read(buffer);
            // ���� ũ�Ⱑ ������ ���� ���� ����.
            if (size == -1) {
                SocketAddress remoteAddr = socket.getRemoteSocketAddress();
                System.out.println("Connection closed by client: " + remoteAddr);
                // ���� ä�� �ݱ�
                channel.close();
                // ���� �ݱ�
                socket.close();
                // Ű �ݱ�
                key.cancel();
                return;
            }
            // ByteBuffer -> byte[]
            byte[] data = new byte[size];
            System.arraycopy(buffer.array(), 0, data, 0, size);
            // StringBuffer ���
            StringBuffer sb = (StringBuffer) key.attachment();
            // ���ۿ� ���ŵ� ������ �߰�
            sb.append(new String(data));
            // ������ ���� ���� �� ���.
            if (sb.length() > 2 && sb.charAt(sb.length() - 2) == CR && sb.charAt(sb.length() - 1) == LF) {
                // ���� ����
                sb.setLength(sb.length() - 2);
                // �޽����� �ֿܼ� ǥ���Ѵ�.
                String msg = sb.toString();
                System.out.println(msg);
                // exit ��� ������ �����Ѵ�.
                if ("exit".equals(msg)) {
                    // ���� ä�� �ݱ�
                    channel.close();
                    // ���� �ݱ�
                    socket.close();
                    // Ű �ݱ�
                    key.cancel();
                    return;
                }
                // Echo - �޽���> �� ���·� �� ����.
                sb.insert(0, "Echo - ");
                sb.append("\r\n>");
                // Socket ä���� channel�� �۽� ����Ѵ�
                channel.register(selector, SelectionKey.OP_WRITE, sb);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // �߽Ž� ȣ�� �Լ�
    private void send(Selector selector, SelectionKey key) {
        try {
            // Ű ä���� �����´�.
            SocketChannel channel = (SocketChannel) key.channel();
            // ä�� Non-blocking ����
            channel.configureBlocking(false);
            // StringBuffer ���
            StringBuffer sb = (StringBuffer) key.attachment();
            String data = sb.toString();
            // StringBuffer �ʱ�ȭ
            sb.setLength(0);
            // byte �������� ��ȯ
            ByteBuffer buffer = ByteBuffer.wrap(data.getBytes());
            // ***������ �۽�***
            channel.write(buffer);
            // Socket ä���� channel�� ���� ����Ѵ�
            channel.register(selector, SelectionKey.OP_READ, sb);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
