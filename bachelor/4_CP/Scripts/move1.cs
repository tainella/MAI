using UnityEngine;
using System.Collections;

// The GameObject is made to bounce using the space key.
// Also the GameOject can be moved forward/backward and left/right.
// Add a Quad to the scene so this GameObject can collider with a floor.

public class move1 : MonoBehaviour
{
    public float speed = 6.0f;
    public float jumpSpeed = 8.0f;
    public float gravity = 20.0f;
    public float pushPower = 2.0f;
    
    private Vector3 moveDirection = Vector3.zero;
    private CharacterController controller;
    
    void Start()
    {
        controller = this.GetComponent<CharacterController>();
    }
    
    void FixedUpdate()
    {
        //if (controller.isGrounded)
        //{
            // We are grounded, so recalculate
            // move direction directly from axes
            float mouseInput = Input.GetAxis("Mouse X");
            Vector3 lookhere = new Vector3(0, mouseInput, 0);
            transform.Rotate(lookhere);
            mouseInput = Input.GetAxis("Mouse Y");
            lookhere = new Vector3(-mouseInput, 0, 0);
            transform.Rotate(lookhere);
            
            moveDirection = new Vector3(Input.GetAxis("Horizontal"), 0.0f, Input.GetAxis("Vertical"));
            moveDirection = transform.TransformDirection(moveDirection);
            moveDirection = moveDirection * speed;
            
            if (Input.GetButton("Jump"))
            {
                moveDirection.y = jumpSpeed;
            }
        //}
        
        // Apply gravity
        moveDirection.y = moveDirection.y - (gravity * Time.deltaTime);
        
        // Move the controller
        controller.Move(moveDirection * Time.deltaTime);
    }
    
    /*
     void OnControllerColliderHit(ControllerColliderHit hit)
    {
        Rigidbody body = hit.collider.attachedRigidbody;
        Debug.Log("HIT");
        // no rigidbody
        if (body == null || body.isKinematic)
        {
            return;
        }
        
        // We dont want to push objects below us
        if (hit.moveDirection.y < -0.3)
        {
            return;
        }
        
        // Calculate push direction from move direction,
        // we only push objects to the sides never up and down
        Vector3 pushDir = new Vector3(hit.moveDirection.x, 0, hit.moveDirection.z);
        
        // If you know how fast your character is trying to move,
        // then you can also multiply the push velocity by that.
        
        // Apply the push
        body.velocity = pushDir * pushPower;
    }
     */
    
    void OnCollisionEnter(Collision myCollision) {
        Debug.Log("HIT");
    }
    
    
}

