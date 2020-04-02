package test;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Scanner;
import java.util.concurrent.Executors;

public class Client implements Runnable {
    private InetSocketAddress connectAddress;
    private static char CR = (char) 0x0D;
    private static char LF = (char) 0x0A;
    private Scanner scanner = new Scanner(System.in);

    // ip�� port ����
    public Client(String address, int port) {
        connectAddress = new InetSocketAddress(address, port);
    }

    // Thread ����.
    public void run() {
        // ������ ����
        try (Selector selector = Selector.open()) {
            // ���� ����
            SocketChannel channel = SocketChannel.open(connectAddress);
            // ä�� Non-blocking ����
            channel.configureBlocking(false);
            // Socket ä���� channel�� �۽� ����Ѵ�
            channel.register(selector, SelectionKey.OP_READ, new StringBuffer());
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
                    if (key.isReadable()) {
                        this.receive(selector, key);
                        // �߽��� ���..
                    } else if (key.isWritable()) {
                        this.send(selector, key);
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // �߽Ž� ȣ�� �Լ�
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
            // ***������ ����***
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
            // ������ ���� ���� �� ���. + >
            if (sb.length() > 3 && sb.charAt(sb.length() - 3) == CR && sb.charAt(sb.length() - 2) == LF
                    && sb.charAt(sb.length() - 1) == '>') {
                // �޽����� �ֿܼ� ǥ��
                String msg = sb.toString();
                System.out.print(msg);
                // StringBuffer �ʱ�ȭ
                sb.setLength(0);
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
            // �ֿܼ��� ���� �Է� �޴´�.
            String msg = scanner.next() + "\r\n";
            ByteBuffer buffer = ByteBuffer.wrap(msg.getBytes());
            // ***������ �۽�***
            channel.write(buffer);
            // Socket ä���� channel�� ���� ����Ѵ�
            channel.register(selector, SelectionKey.OP_READ, key.attachment());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}