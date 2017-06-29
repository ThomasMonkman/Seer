import { Component, OnInit } from '@angular/core';
import { SeerConnectionService } from "app/services/seer-connection/seer-connection.service";
import { Observable } from "rxjs/Rx";

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {

  protected data: any[] = [];
  private seerObservable: Observable<any>;
  constructor(private seerConnection: SeerConnectionService) { }

  // Dynamic tabs demo
  activeTabIndex = 0;
  addTabPosition = 0;
  gotoNewTabAfterAdding = false;
  createWithLongContent = false;
  dynamicTabs = [
    {
      label: 'Tab 1',
      content: 'This is the body of the first tab'
    }, {
      label: 'Tab 2',
      disabled: true,
      content: 'This is the body of the second tab'
    }, {
      label: 'Tab 3',
      extraContent: true,
      content: 'This is the body of the third tab'
    }, {
      label: 'Tab 4',
      content: 'This is the body of the fourth tab'
    },
  ];

  public ngOnInit() {
    // this.seerObservable = this.seerConnection.connect('ws://localhost:9000');
    // this.seerObservable.subscribe((message: any) => {
    //   console.log('received message from server: ', message);
    //   this.data.push(message);
    // });
  }

  protected deleteTab(tab: any) {
    this.dynamicTabs.splice(this.dynamicTabs.indexOf(tab), 1);
  }
}
