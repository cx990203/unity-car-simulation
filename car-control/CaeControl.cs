using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CaeControl : MonoBehaviour
{
    private const string HORIZONTAL = "Horizontal";
    private const string VERTICAL = "Vertical";

    private Vector3 startPosition;
    private float horizontalInput;
    private float verticalInput;
    private float currentSteerAngle;
    private float currentbreakForce;
    private bool isBreaking;

    [SerializeField] private float motorForce;
    [SerializeField] private float breakForce;
    [SerializeField] private float maxSteeringAngle;

    [SerializeField] private WheelCollider frontLeftWheelCollider;
    [SerializeField] private WheelCollider frontRightWheelCollider;
    [SerializeField] private WheelCollider rearLeftWheelCollider;
    [SerializeField] private WheelCollider rearRightWheelCollider;

    [SerializeField] private Transform frontLeftWheelTransform;
    [SerializeField] private Transform frontRightWheelTransform;
    [SerializeField] private Transform rearLeftWheelTransform;
    [SerializeField] private Transform rearRightWheelTransform;

    [SerializeField] private Rigidbody carRigidbody;

    void Start()
    {
        /* 获取车辆初始位置 */
        startPosition = transform.position;
    }

    void Update()
    {
        Vector3 pos = carRigidbody.transform.position;
        if (pos[1] < -10)
        {
            /* 使车辆停下来 */
            carRigidbody.velocity = Vector3.zero;
            carRigidbody.constraints = RigidbodyConstraints.FreezeAll;
            carRigidbody.constraints = RigidbodyConstraints.None;
            /* 让车辆调整角度后回到原点 */
            carRigidbody.transform.rotation = new Quaternion(0, 0, 0, 0);
            carRigidbody.transform.position = startPosition;
        }

    }

    private void FixedUpdate()
    {
        /* 车辆控制 */
        GetInput();         // 获取键盘输入
        HandleMotor();      // 将键盘输入转换为点击输出
        HanlderSteering();  // 将键盘输入转换为舵机输出
        UpdateWheels();     // 对车轮数值进行更新
    }

    private void HandleMotor()
    {
        rearLeftWheelCollider.motorTorque = verticalInput * motorForce;
        rearRightWheelCollider.motorTorque = verticalInput * motorForce;
        currentbreakForce = isBreaking ? breakForce : 0f;
        ApplyBreaking();
    }

    private void ApplyBreaking()
    {
        frontLeftWheelCollider.brakeTorque = currentbreakForce;
        frontRightWheelCollider.brakeTorque = currentbreakForce;
        rearLeftWheelCollider.brakeTorque = currentbreakForce;
        rearRightWheelCollider.brakeTorque = currentbreakForce;
    }

    private void HanlderSteering()
    {
        currentSteerAngle = maxSteeringAngle * horizontalInput;
        frontLeftWheelCollider.steerAngle = currentSteerAngle;
        frontRightWheelCollider.steerAngle = currentSteerAngle;
    }

    private void GetInput()
    {
        horizontalInput = Input.GetAxis(HORIZONTAL); /* 键盘上的← →按键，得到的值为（-1，1） */
        verticalInput = Input.GetAxis(VERTICAL);    /* 键盘上的↑ ↓按键，得到的值为（-1，1） */
        isBreaking = Input.GetKey(KeyCode.Space);
    }

    private void UpdateWheels()
    {
        UpdateSingleWheel(frontLeftWheelCollider, frontLeftWheelTransform);
        UpdateSingleWheel(frontRightWheelCollider, frontRightWheelTransform);
        UpdateSingleWheel(rearLeftWheelCollider, rearLeftWheelTransform);
        UpdateSingleWheel(rearRightWheelCollider, rearRightWheelTransform);
    }

    private void UpdateSingleWheel(WheelCollider wheelCollider, Transform wheelTransform)
    {
        Vector3 pos;
        Quaternion rot;
        wheelCollider.GetWorldPose(out pos, out rot);
        wheelTransform.rotation = rot;
        wheelTransform.position = pos;
    }
}
