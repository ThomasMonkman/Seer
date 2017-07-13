import { Component, OnInit } from '@angular/core';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { SeerConnectionService } from 'app/services/seer-connection/seer-connection.service';
import { Tab } from 'app/views/dashboard/tab';
import { Observable } from 'rxjs/Rx';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent {

  protected tabs: Tab[] = [new Tab()]; // Default to a new tab
  protected activeTabIndex: number = 0;

  protected newConnection(newConnection: ConnectionMeta) {
    console.log('tab added');
    this.tabs.splice(this.tabs.length - 1, 0, new Tab(newConnection));
  }

  protected deleteTab(tab: any) {
    this.tabs.splice(this.tabs.indexOf(tab), 1);
  }
}
