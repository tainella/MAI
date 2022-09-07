using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class exitbutton : MonoBehaviour {
    void Start () {
        Button btn = this.GetComponent<Button>();
        btn.onClick.AddListener(TaskOnClick);
    }
    
    void TaskOnClick(){
        Debug.Log("Has quit");
        Application.Quit();
    }
}
