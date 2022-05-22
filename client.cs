using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System;
using System.Text;
using System.IO.MemoryMappedFiles;
using System.Threading;
using System.IO;

public class client : MonoBehaviour
{
    //socket参数
    Socket CameraClient;
    public IPEndPoint ipendPoint;

    //固定参数
    const string cameraFileName = "CarCamera.cam";
    const string locFileName = "data";       //由于采用内存池方法设计，因此只定义文件名字，不定义文件类型
    const string endSymbol = "\t";              //消息结束符
    const float pi = 3.1415926f;

    //运行参数
    bool connect_flag = false;          //服务器连接标志位

    //unity交互控件
    [SerializeField] private RenderTexture target;
    [SerializeField] private Camera CarCamera;
    [SerializeField] private Transform Ladar;
    [SerializeField] private bool UsingLadar_16;
    /* ---------------------------------------------------初始化--------------------------------------------------- */
    // Start is called before the first frame update
    void Start()
    {
        /* 设置客户端接口 */
        CameraClient = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        /* 连接服务器 */
        ConnectToServer();
        ///* 创建雷达线程 */
        //ThreadStart ladarRef = new ThreadStart(LadarThread);
        //Thread ladarThread = new Thread(ladarRef);
        //ladarThread.Start();
        ///* 创建相机线程 */
        //ThreadStart CameraRef = new ThreadStart(CameraThread);
        //Thread cameraThread = new Thread(CameraRef);
        //cameraThread.Start();
    }

    /* ---------------------------------------------------连接服务器相关API--------------------------------------------------- */
    /// <summary>
    /// 连接服务器
    /// </summary>
    public void ConnectToServer()
    {
        ipendPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8080);
        Debug.Log("开始链接服务器......");
        //请求链接
        CameraClient.BeginConnect(ipendPoint, ConnectCallback, "");
    }

    /// <summary>
    /// 链接的回调
    /// </summary>
    /// <param name="ar"></param>
    public void ConnectCallback(IAsyncResult ar)
    {
        try
        {
            CameraClient.EndConnect(ar);
        }
        catch (Exception e)
        {
            Debug.Log(e.ToString());
        }
        //判断到底链接成功了还是没有
        if (CameraClient.Connected == true)
        {
            //调用链接成功的回调
            Debug.Log("连接成功");
            connect_flag = true;
        }
        else
        {
            //链接失败
            Debug.Log("连接失败");
            connect_flag = false;
        }
    }
    /* ---------------------------------------------------循环更新--------------------------------------------------- */
    // Update is called once per frame
    //int not_connect_frame = 0;          //记录没有连接的帧数
    string loc_filename;                    //数据保存文件名
    void Update()
    {
        /* 保存图片至本地 */
        OnUserSave(cameraFileName);
        //SaveImageToJPG(50);           //保存相机图片(.jpg)
        if (!UsingLadar_16)
        {
            /*--如果使用单个雷达--*/
            /* 获取雷达数据 */
            Vector3 L_point = getLaderPoint();
            /* 保存数据 */
            loc_filename = savelocData(L_point);
        }
        else if (UsingLadar_16)
        {
            /*--如果使用16线雷达--*/
            /* 获取雷达数据 */
            Vector3[] L_point = getLaderPoint_16();
            /* 保存数据 */
            loc_filename = savelocData_16(L_point);
        }
        else return;
        /* 如果连接上了服务器 */
        if (connect_flag)
        {
            /* 发送消息
             * 数据格式：
             *      imageHead:<cameraFileName><endSymbol>loc:<loc_filename><endSymbol>
             */
            /* 定义发送消息 */
            string mes = "imageHead:" + cameraFileName + endSymbol;       //图片文件数据消息
            mes += "locHead:" + loc_filename + endSymbol;                     //位置信息数据消息
            /* 发送信息 */
            try
            {
                PutMessageToQueue(mes);
            }
            catch (SocketException e)
            {
                /* 如果发送错误，则断开连接 */
                Debug.Log("断开与服务器的连接");
                connect_flag = false;
            }
        }
        else {
            /* 如果没有连接服务器，则不断请求连接 */
            //not_connect_frame++;
            //if (not_connect_frame >= 200) {
            //    CameraClient.BeginConnect(ipendPoint, ConnectCallback, "");
            //    Debug.Log("尝试重新链接服务器......");
            //    not_connect_frame = 0;
            //}
            
        }
    }

    /* ---------------------------------------------------多线程循环函数--------------------------------------------------- */
    /* 雷达数据获取线程 */
    public void LadarThread()
    {
        while (true)
        {
            /* 获取雷达数据 */
            //Vector3 point = getLaderPoint();
        }
    }

    /* 相机线程 */
    public void CameraThread()
    {
        while (true)
        {
            
        }
    }

    /* ---------------------------------------------------服务器消息发送API--------------------------------------------------- */
    /// <summary>
    /// 发送消息的回调
    /// </summary>
    /// <param name="ar"></param>
    public void SendMessageCallback(IAsyncResult ar)
    {
        //停止发送
        int length = CameraClient.EndSend(ar);
        //Debug.Log("发送的长度：" + length);
    }

    /// <summary>
    /// 发送消息给服务器
    /// </summary>
    /// <param name="sendMsgContent"></param>
    /// <param name="offset"></param>
    /// <param name="size"></param>
    public void SendBytesMessageToServer(byte[] sendMsgContent, int offset, int size)
    {
        CameraClient.BeginSend(sendMsgContent, offset, size, SocketFlags.None, SendMessageCallback, "");
    }

    /// <summary>
    /// 发送消息到队列
    /// </summary>
    /// <param name="msg"></param>
    public void PutMessageToQueue(string msg)
    {
        //将对象序列化发过去
        byte[] msgBytes = ASCIIEncoding.UTF8.GetBytes(msg);
        SendBytesMessageToServer(msgBytes, 0, msgBytes.Length);
    }


    /* ---------------------------------------------------图像保存相关API--------------------------------------------------- */
    int frame = 0, image_index = 0;
    public void SaveImageToJPG(int fcount)
    {
        frame++;
        if (frame >= fcount)
        {
            /* 获取保存文件路径 */
            string filename = image_index + string.Format(".jpg");
            var prePath = Application.dataPath.Substring(0, Application.dataPath.LastIndexOf("/"));
            string path = prePath + string.Format("/Assets/image/Save/") + filename;
            Debug.Log(path);
            /* 获取保存文件数据 */
            Texture2D texture2D = CreateFrom(target);
            var bytes = texture2D.EncodeToJPG();
            System.IO.File.WriteAllBytes(path, bytes);
            /* 帧数以及保存有图片数量修改 */
            frame = 0;
            image_index++;
        }
        
    }

    public void OnUserSave(string filename)
    {
        var prePath = Application.dataPath.Substring(0, Application.dataPath.LastIndexOf("/"));
        string path = prePath + string.Format("/Assets/__buffer__/") + filename;
        try
        {
            Save(path, CreateFrom(target));
        }
        catch (IOException e)
        {

        }
        
    }

    public void Save(string path, Texture2D texture2D)
    {
        var bytes = texture2D.EncodeToPNG();
        //var bytes = texture2D.EncodeToJPG();
        System.IO.File.WriteAllBytes(path, bytes);
    }

    public Texture2D CreateFrom(RenderTexture renderTexture)
    {
        Texture2D texture2D = new Texture2D(renderTexture.width, renderTexture.height, TextureFormat.ARGB32, false);
        var previous = RenderTexture.active;
        RenderTexture.active = renderTexture;

        texture2D.ReadPixels(new Rect(0, 0, renderTexture.width, renderTexture.height), 0, 0);

        RenderTexture.active = previous;

        texture2D.Apply();

        return texture2D;
    }

    /* ---------------------------------------------------雷达相关API--------------------------------------------------- */
    const int maxDis = 100;         // max distance，雷达的最大距离
    /// <summary>
    /// 单个雷达
    /// </summary>
    /// <returns></returns>
    public Vector3 getLaderPoint()
    {
        /* 获取雷达点 */
        Ladar.Rotate(new Vector3(0, 3.1f / 60.0f, 0));         //雷达旋转
        Ray ray = new Ray(Ladar.position, Ladar.forward * maxDis);
        RaycastHit hit;
        if (Physics.Raycast(ray, out hit, maxDis))
        {
            /* 如果雷达射线有碰撞 */
            Debug.DrawLine(Ladar.position, hit.point, Color.red);
            return hit.point;
        }

        Debug.DrawLine(Ladar.position, Ladar.position + Ladar.forward * maxDis, Color.red);
        return new Vector3(0, 0, 0);
    }

    /// <summary>
    /// 十六线雷达
    /// </summary>
    /// <returns></returns>
    public Vector3[] getLaderPoint_16()
    {
        /* 获取雷达点 */
        Ladar.Rotate(new Vector3(0, 0, 0));         //雷达旋转
        Vector3[] res = new Vector3[16];        //三维向量的数组，用于存储雷达信息
        const float angle_max = 8;         //雷达最大角度

        for (int i = 0; i < 8; i++)
        {
            /* 雷达向上旋转 */
            Vector3 dir_up = Vector3Rotate(Ladar.forward, Vector3.left, angle_max / 8.0f * i);          //确定雷达方向
            Ray ray_up = new Ray(Ladar.position, dir_up * maxDis);              //定义雷达射线
            RaycastHit hit_up;          //定义射线碰撞体
            if (Physics.Raycast(ray_up, out hit_up, maxDis))
            {
                /* 如果雷达射线有碰撞 */
                Debug.DrawLine(Ladar.position, hit_up.point, Color.blue);
                res[i * 2] = hit_up.point;
            }
            else
                Debug.DrawLine(Ladar.position, Ladar.position + dir_up * maxDis, Color.blue);
            /* 雷达向下旋转 */
            Vector3 dir_down = Vector3Rotate(Ladar.forward, Vector3.left, -angle_max / 8.0f * i);        //确定雷达方向
            Ray ray_down = new Ray(Ladar.position, dir_down * maxDis);          //定义雷达射线
            RaycastHit hit_down;            //定义射线碰撞体
            if (Physics.Raycast(ray_down, out hit_down, maxDis))
            {
                /* 如果雷达射线有碰撞 */
                Debug.DrawLine(Ladar.position, hit_down.point, Color.blue);
                res[i * 2 + 1] = hit_down.point;
            }
            else
                Debug.DrawLine(Ladar.position, Ladar.position + dir_down * maxDis, Color.blue);
        }

        return res;
    }

    int index = 0;
    public string savelocData(Vector3 p)
    {
        /* 
         * 保存点至指定文件 
         * 采用内存池设计，内存池分为10个区，index数值为：0-10
         * 这样写入部分和调用部分可以分开
         * 返回保存文件名
         */
        const string file_type = ".loc";        //文件后缀
        var prePath = Application.dataPath.Substring(0, Application.dataPath.LastIndexOf("/"));         //获取当前工程路径
        string path = prePath + "/Assets/__buffer__/__loc__/" + locFileName + index + file_type;        //存储文件路径
        /* 如果文件不存在，则创建文件 */
        if (!File.Exists(path))
        {
            File.Create(path).Dispose();
        }
        /* 获取文件指针 */
        FileInfo f = new FileInfo(path);
        /* 创建数据流，重新编辑文件 */
        StreamWriter sw = f.CreateText();
        /* 保存当前点坐标 */
        Transform ladar_t = Ladar;
        sw.WriteLine(string.Format("{0:F2},", ladar_t.position[0]) + string.Format("{0:F2},", ladar_t.position[1]) + string.Format("{0:F2}", ladar_t.position[2]));
        /* 保存雷达数据 */
        sw.WriteLine(string.Format("{0:F2},", p[0]) + string.Format("{0:F2},", p[1]) + string.Format("{0:F2}", p[2]));
        sw.Close();

        index++;
        index = index < 10 ? index : 0;
        return locFileName + index + file_type;
    }


    public string savelocData_16(Vector3[] p)
    {
        /* 
         * 功能与savelocData一致，只不过保存的是16线雷达
         */
        const string file_type = ".loc";            //文件后缀
        var prePath = Application.dataPath.Substring(0, Application.dataPath.LastIndexOf("/"));         //获取当前工程信息
        string path = prePath + "/Assets/__buffer__/__loc16__/" + locFileName + index + file_type;      //存储文件路径
        /* 如果文件不存在，则创建文件 */
        if (!File.Exists(path))
        {
            File.Create(path).Dispose();
        }
        /* 获取文件指针 */
        FileInfo f = new FileInfo(path);
        /* 创建数据流，重新编辑文件 */
        StreamWriter sw = f.CreateText();
        /* 保存当前点坐标 */
        Transform ladar_t = Ladar;
        sw.WriteLine(string.Format("{0:F2},", ladar_t.position[0]) + string.Format("{0:F2},", ladar_t.position[1]) + string.Format("{0:F2}", ladar_t.position[2]));
        /* 保存16线雷达数据 */
        for (int i = 0; i < 16; i++)
        {
            sw.WriteLine(string.Format("{0:F2},", p[i][0]) + string.Format("{0:F2},", p[i][1]) + string.Format("{0:F2}", p[i][2]));
        }
        sw.Close();

        index++;
        index = index < 10 ? index : 0;
        return locFileName + index + file_type;
    }

    /* ---------------------------------------------------共享内存相关API--------------------------------------------------- */
    void MyMmp(string str)
    {
        //// 长度
        //long t = 100;
        //// 创建内存块，test1,其他语言利用这个内存块名字就能找到内存块。
        //var mmf = MemoryMappedFile.CreateOrOpen("CarCamera", t, MemoryMappedFileAccess.ReadWrite);
        //var viewAccessor = mmf.CreateViewAccessor(0, t);
        //viewAccessor.Write(0, str.Length); ;
        //viewAccessor.WriteArray<byte>(0, System.Text.Encoding.Default.GetBytes(str), 0, str.Length);
        int size = 1024;
        MemoryMappedFile shareMemory = MemoryMappedFile.CreateOrOpen("CarCamera", size);
        var stream = shareMemory.CreateViewStream(0, size);
        byte[] data = System.Text.Encoding.UTF8.GetBytes(str);
        stream.Write(data, 0, data.Length);
        stream.Dispose();
    }


    /* ---------------------------------------------------工具函数--------------------------------------------------- */
    /// <summary>
    /// 围绕某点旋转指定角度
    /// </summary>
    /// <param name="vector">旋转向量</param>
    /// <param name="axis">围绕旋转轴</param>
    /// <param name="angle">旋转角度</param>
    /// <returns></returns>
    public Vector3 Vector3Rotate(Vector3 vector, Vector3 axis, float angle)
    {
        return Quaternion.AngleAxis(angle, axis) * vector;
    }
}
