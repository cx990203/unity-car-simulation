using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Diagnostics;

public class pytest : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        RunPythonScript("main.py");
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public static void RunPythonScript(string sArgName, string args = "")
    {
        Process p = new Process();
        //python脚本的路径
        string path = Application.dataPath + @"/pyScripts/" + sArgName;
        string sArguments = path;


        //(注意：用的话需要换成自己的)没有配环境变量的话，可以像我这样写python.exe的绝对路径
        //(用的话需要换成自己的)。如果配了，直接写"python.exe"即可
        p.StartInfo.FileName = @"F:/software/Anaconda3/envs/pytorch/python.exe";
        //p.StartInfo.FileName = @"C:\Program Files\Python35\python.exe";

        // sArguments为python脚本的路径   python值的传递路线strArr[]->teps->sigstr->sArguments 
        //在python中用sys.argv[ ]使用该参数
        p.StartInfo.UseShellExecute = false;
        p.StartInfo.Arguments = sArguments;
        p.StartInfo.RedirectStandardOutput = true;
        p.StartInfo.RedirectStandardInput = true;
        p.StartInfo.RedirectStandardError = true;
        p.StartInfo.CreateNoWindow = true;
        p.Start();
        p.BeginOutputReadLine();
        p.OutputDataReceived += new DataReceivedEventHandler(Out_RecvData);
        //Console.ReadLine();
        //p.WaitForExit();
    }

    static void Out_RecvData(object sender, DataReceivedEventArgs e)
    {
        if (!string.IsNullOrEmpty(e.Data))
        {
            UnityEngine.Debug.Log(e.Data);
        }
    }
}
