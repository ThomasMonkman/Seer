import { Component, OnInit } from '@angular/core';
import { SeerConnectionService } from "app/services/seer-connection/seer-connection.service";
import { Observable } from "rxjs/Rx";
import { Tab } from 'app/views/dashboard/tab';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {

  protected data: any[] = [];
  private seerObservable: Observable<any>;

  // Dynamic tabs demo
  protected activeTabIndex = 0;
  protected addTabPosition = 0;
  protected gotoNewTabAfterAdding = false;
  protected createWithLongContent = false;
  protected dynamicTabs: Tab[] = [
    new Tab('Localhost')
  ];

  constructor(private seerConnection: SeerConnectionService) { 
    
  }

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
